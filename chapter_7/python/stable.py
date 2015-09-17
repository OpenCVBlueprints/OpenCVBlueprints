#!/usr/bin/env python

import tornado.web
import cv2
import sys, math, os

import calibration_worker
import utilities
import numpy as np
import matplotlib.pyplot as plt

MAX_FRAMES = 1000
DO_ROLLING_SHUTTER = False

class GyroVideo(object):
    def __init__(self, mp4):
        self.mp4 = mp4
        self.frameInfo = []
        self.numFrames = 0
        self.duration = 0
        self.frameWidth = 0
        self.frameHeight = 0

    def read_video(self, skip_keypoints=False):
        """
        Extracts the keypoints out of a video
        """
        vidcap = cv2.VideoCapture(self.mp4)

        success, frame = vidcap.read()
        prev_frame = None
        previous_timestamp = 0
        frameCount = 0

        self.frameWidth = frame.shape[1]
        self.frameHeight = frame.shape[0]

        while success:
            current_timestamp = None
            current_timestamp = vidcap.get(0) * 1000 * 1000
            print "Processing frame#%d (%f ns)" % (frameCount, current_timestamp)

            if frameCount == 0:
                store = {'keypoints': None, 'timestamp': current_timestamp}
                print store
                self.frameInfo.append(store)
                frameCount += 1
                previous_timestamp = current_timestamp
                prev_frame = frame
                success, frame = vidcap.read()
                continue

            if skip_keypoints:
                store = {'keypoints': None, 'timestamp': current_timestamp}
                print store
                self.frameInfo.append(store)
                continue

            old_gray = cv2.cvtColor(prev_frame, cv2.COLOR_BGR2GRAY)

            new_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            old_corners = cv2.goodFeaturesToTrack(old_gray, 1000, 0.3, 30)

            if old_corners == None:
                # No keypoints?
                store = {'keypoints': None, 'timestamp': current_timestamp}
                print store
                self.frameInfo.append(store)
                frameCount += 1
                previous_timestamp = current_timestamp
                prev_frame = frame
                success, frame = vidcap.read()
                continue

            new_corners, status, err = cv2.calcOpticalFlowPyrLK(old_gray, new_gray, old_corners, None, winSize=(15, 15), maxLevel=2, criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 0.03))


            if len(old_corners) > 4:
                # Try and find the perfect matches
                homography, mask = cv2.findHomography(old_corners, new_corners, cv2.RANSAC, 5.0)
                mask = mask.ravel()
                new_corners_homography = np.asarray([new_corners[i] for i in xrange(len(mask)) if mask[i] == 1])
                old_corners_homography = np.asarray([old_corners[i] for i in xrange(len(mask)) if mask[i] == 1])
            else:
                new_corners_homography = new_corners
                old_corners_homography = old_corners

            pf = prev_frame.copy()
            nf = frame.copy()
            for old, new in zip(old_corners_homography, new_corners_homography):
                pt_old = (old[0][0], old[0][1])
                pt_new = (new[0][0], new[0][1])
                cv2.line(pf, pt_old, pt_old, (0, 0, 255), 5)
                cv2.line(nf, pt_new, pt_new, (0, 0, 255), 5)
            cv2.imwrite("/tmp/ttt%04d-a.png" % frameCount, pf)
            cv2.imwrite("/tmp/ttt%04d-b.png" % frameCount, nf)
            pf = None
            nf = None

            if len(new_corners_homography) != len(new_corners):
                print "ELIMINATED SOME POINTS"

            # For frame n, see what keypoints existed on n-1 and
            # where they exist now
            self.frameInfo.append({'keypoints': (old_corners_homography, new_corners_homography), 'timestamp': current_timestamp})
            frameCount += 1
            previous_timestamp = current_timestamp
            prev_frame = frame
            success, frame = vidcap.read()

            if frameCount == MAX_FRAMES:
                break

        self.numFrames = frameCount
        self.duration = current_timestamp

        return

def fetch_closest_trio(theta_x, theta_y, theta_z, timestamps, req_timestamp):
    """
    Returns the closest match for a given timestamp
    """
    try:
        if req_timestamp in timestamps:
            indexOfTimestamp = timestamps.index(req_timestamp)
            return ( (theta_x[indexOfTimestamp], theta_y[indexOfTimestamp], theta_z[indexOfTimestamp]), req_timestamp, None)
    except IndexError, e:
        import pdb; pdb.set_trace()

    i = 0
    sorted_keys = sorted(timestamps)
    for ts in sorted_keys:
        if ts > req_timestamp:
            break

        i += 1

    # We're looking for the ith and the i+1th req_timestamp
    t_previous = sorted_keys[i-1]
    t_current = sorted_keys[i]
    dt = float(t_current - t_previous)

    slope = (req_timestamp - t_previous) / dt

    t_previous_index = timestamps.index(t_previous)
    t_current_index = timestamps.index(t_current)

    new_x = theta_x[t_previous_index] * (1-slope) + theta_x[t_current_index]*slope
    new_y = theta_y[t_previous_index] * (1-slope) + theta_y[t_current_index]*slope
    new_z = theta_z[t_previous_index] * (1-slope) + theta_z[t_current_index]*slope

    return ((new_x, new_y, new_z), t_previous, t_current)

def getRodrigues(rx, ry, rz):
    smallR = cv2.Rodrigues(np.array([float(rx), float(ry), float(rz)]))[0]
    R = np.array([[smallR[0][0], smallR[0][1], smallR[0][2], 0],
                  [smallR[1][0], smallR[1][1], smallR[1][2], 0],
                  [smallR[2][0], smallR[2][1], smallR[2][2], 0],
                  [0,         0,         0,         1]])
    return R


def getAccumulatedRotation(w, h, theta_x, theta_y, theta_z, timestamps, prev, current, f, gyro_delay=None, gyro_drift=None, shutter_duration=None, doSub=False):

    if not gyro_delay:
        gyro_delay = 0

    if not gyro_drift:
        gyro_drift = (0, 0, 0)

    if not shutter_duration:
        shutter_duration = 0

    x = np.array([[1, 0, -w/2],
                      [0, 1, -h/2],
                      [0, 0, 0],
                      [0, 0, 1]])
    A1 = np.asmatrix(x)
    transform = A1.copy()

    prev = prev + gyro_delay
    current = current + gyro_delay

    if prev in timestamps and current in timestamps:
        start_timestamp = prev
        end_timestamp = current
    else:
        (rot_start, start_timestamp, t_next) = fetch_closest_trio(theta_x, theta_y, theta_z, timestamps, prev)
        (rot_end, end_timestamp, t_next) = fetch_closest_trio(theta_x, theta_y, theta_z, timestamps, current)

    gyro_drifted = (float(rot_end[0] + gyro_drift[0]),
                    float(rot_end[1] + gyro_drift[1]),
                    float(rot_end[2] + gyro_drift[2]))
    if doSub:
        gyro_drifted = (gyro_drifted[0] - rot_start[0],
                        gyro_drifted[1] - rot_start[1],
                        gyro_drifted[2] - rot_start[2])
    R = getRodrigues(gyro_drifted[1], -gyro_drifted[0], -gyro_drifted[2])

    x = np.array([[1.0, 0, 0, 0],
                     [0, 1.0, 0, 0],
                     [0, 0, 1.0, f],
                     [0, 0, 0, 1.0]])
    T = np.asmatrix(x)
    x = np.array([[f, 0, w/2, 0],
                  [0, f, h/2, 0],
                  [0, 0, 1, 0]])
    transform = R*(T*transform)

    A2 = np.asmatrix(x)

    transform = A2 * transform

    return transform

def accumulateRotation(src, theta_x, theta_y, theta_z, timestamps, prev, current, f, gyro_delay=None, gyro_drift=None, shutter_duration=None):
    if prev == current:
        return src

    pts = []
    for x in range(10):
        current_row = []
        current_row_transformed = []
        pixel_x = x * (src.shape[1] / 10)
        for y in range(10):
            pixel_y = y * (src.shape[0] / 10)
            current_row.append( [pixel_x, pixel_y] )

            if shutter_duration:
                y_timestamp = current + shutter_duration * (pixel_y - src.shape[0]/2)
            else:
                y_timestamp = current

            transform = getAccumulatedRotation(src.shape[1], src.shape[0], theta_x, theta_y, theta_z, timestamps, prev, current, f, gyro_delay, gyro_drift)
            output = cv2.perspectiveTransform(np.array([[pixel_x, pixel_y]], transform)
            current_row_transformed.append(output)

        pts.append(current_row)
        pts_transformed.append(current_row_transformed)

    o = utilities.meshwarp(src, pts_transformed)
    return o

    

def rotateImage(src, rx, ry, rz, dx, dy, dz, f, convertToRadians=False):
    if convertToRadians:
        rx = (rx) * math.pi / 180
        ry = (ry) * math.pi / 180
        rz = (rz) * math.pi / 180

    rx = float(rx)
    ry = float(ry)
    rz = float(rz)

    w = src.shape[1]
    h = src.shape[0]

    x = np.array([[1, 0, -w/2],
                      [0, 1, -h/2],
                      [0, 0, 0],
                      [0, 0, 1]])

    A1 = np.asmatrix(x)
    smallR = cv2.Rodrigues(np.array([rx, ry, rz]))[0]
    
    R = np.array([[smallR[0][0], smallR[0][1], smallR[0][2], 0],
                     [smallR[1][0], smallR[1][1], smallR[1][2], 0],
                     [smallR[2][0], smallR[2][1], smallR[2][2], 0],
                     [0,         0,         0,         1]])


    x = np.array([[1.0, 0, 0, dx],
                     [0, 1.0, 0, dy],
                     [0, 0, 1.0, dz],
                     [0, 0, 0, 1.0]])
    T = np.asmatrix(x)

    x = np.array([[f, 0, w/2, 0],
                     [0, f, h/2, 0],
                     [0, 0, 1, 0]])
    A2 = np.asmatrix(x)

    transform = A2*(T*(R*A1))
    o = cv2.warpPerspective(src, transform, (w, h))
    return o


def render_trio(signal_x, signal_y, signal_z, timestamps):
    plt.plot(timestamps, signal_x, 'b-', timestamps, signal_y, 'g-', timestamps, signal_z, 'r-')
    plt.ylabel("Y")
    plt.show()

class CalibrateGyroStabilize(object):
    def get_gaussian_kernel(self, sigma2, v1, v2, normalize=True):
        gauss = [math.exp(-(float(x*x) / sigma2)) for x in range(v1, v2+1)]
        total = sum(gauss)

        if normalize:
            gauss = [x/total for x in gauss]

        return gauss

    def gaussian_filter(self, input_array, sigma=10000, r=256):
        """
        """
        # Step 1: Define the convolution kernel
        kernel = self.get_gaussian_kernel(sigma, -r, r)

        # Step 2: Convolve
        return np.convolve(input_array, kernel, 'same')

    def calcErrorScore(self, set1, set2):
        if len(set1) != len(set2):
            raise Exception("The given two sets don't have the same length")

        score = 0
        set1 = [ (x[0][0], x[0][1]) for x in set1.tolist() ]
        if not DO_ROLLING_SHUTTER:
            set2 = [ (x[0][0], x[0][1]) for x in set2.tolist() ]

        for first, second in zip(set1, set2):
            diff_x = math.pow(first[0] - second[0], 2)
            diff_y = math.pow(first[1] - second[1], 2)

            score += math.sqrt(diff_x + diff_y)

        return score

    def calcErrorAcrossVideo(self, videoObj, theta, timestamps, focal_length, gyro_delay=None, gyro_drift=None, rolling_shutter=None):
        total_error = 0
        frame_height = videoObj.frameHeight
        for frameCount in xrange(videoObj.numFrames):
            frameInfo = videoObj.frameInfo[frameCount]
            current_timestamp = frameInfo['timestamp']

            if frameCount == 0:
                # INCRMENT
                #frameCount += 1
                previous_timestamp = current_timestamp
                continue

            keypoints = frameInfo['keypoints']
            if keypoints:
                old_corners = frameInfo['keypoints'][0]
                new_corners = frameInfo['keypoints'][1]
            else:
                # Don't use this for calculating errors
                continue

            # Ideally, after our transformation, we should get points from the
            # thetas to match new_corners

            #########################
            # Step 0: Work with current parameters and calculate the error score

            transformed_corners = []
            if DO_ROLLING_SHUTTER:
                for pt in old_corners:
                    x = pt[0][0]
                    y = pt[0][1]

                    # The time when this pixel was captured - the timestamp is centered around the
                    # the center
                    pt_timestamp = int(current_timestamp) + rolling_shutter*(y-frame_height/2)/frame_height
                    
                    transform = getAccumulatedRotation(videoObj.frameWidth, videoObj.frameHeight, theta[0], theta[1], theta[2], timestamps, int(previous_timestamp), int(pt_timestamp), focal_length, gyro_delay, gyro_drift, doSub=True)
                    output = transform * np.matrix("%f;%f;1.0" % (x, y)).tolist()
                    tx = (output[0][0]/output[2][0]).tolist()[0][0]
                    ty = (output[1][0]/output[2][0]).tolist()[0][0]
                    transformed_corners.append( np.array([tx, ty]) )
            else:
                transform = getAccumulatedRotation(videoObj.frameWidth, videoObj.frameHeight, theta[0], theta[1], theta[2], timestamps, int(previous_timestamp), int(current_timestamp), focal_length, gyro_delay, gyro_drift, doSub=True)
                transformed_corners = cv2.perspectiveTransform(old_corners, transform)

            error = self.calcErrorScore(new_corners, transformed_corners)

            #print "Error(%d) = %f" % (frameCount, error)

            total_error += error

            # For a random frame - write out the outputs
            if frameCount == MAX_FRAMES / 2:
                img = np.zeros( (videoObj.frameHeight, videoObj.frameWidth, 3), np.uint8)
                for old, new, transformed in zip(old_corners, new_corners, transformed_corners):
                    pt_old = (old[0][0], old[0][1])
                    pt_new = (new[0][0], new[0][1])
                    pt_transformed = (transformed[0][0], transformed[0][1])
                    cv2.line(img, pt_old, pt_old, (0, 0, 255), 2)
                    cv2.line(img, pt_new, pt_new, (0, 255, 0), 1)
                    cv2.line(img, pt_transformed, pt_transformed, (0, 255, 255), 1)
                cv2.imwrite("/tmp/ddd%04d-a.png" % frameCount, img)

            # INCRMENT
            #frameCount += 1
            previous_timestamp = current_timestamp

        return total_error

    def calcErrorAcrossVideoObjective(self, parameters, videoObj, theta, timestamps):
        """
        Wrapper function for scipy
        """
        focal_length = float(parameters[0])
        gyro_delay = float(parameters[1])
        gyro_drift = ( float(parameters[2]), float(parameters[3]), float(parameters[4]) )
        rolling_shutter = float(parameters[5])

        #print "Focal length = %f" % focal_length
        #print "gyro_delay = %f" % gyro_delay
        #print "gyro_drift = (%f, %f, %f)" % gyro_drift

        error = self.calcErrorAcrossVideo(videoObj, theta, timestamps, focal_length, gyro_delay, gyro_drift, rolling_shutter)
        print "Error = %f" % (error / videoObj.numFrames)
        return error

    def diff(self, timestamps):
        """
        Returns differences between consecutive elements
        """
        return np.ediff1d(timestamps)


    def __init__(self, mp4, csv):
        self.mp4 = mp4
        self.csv = csv

    def calibrate(self):
        gdf = calibration_worker.GyroscopeDataFile(self.csv)
        gdf.parse()

        signal_x = gdf.get_signal_x()
        signal_y = gdf.get_signal_y()
        signal_z = gdf.get_signal_z()
        timestamps = gdf.get_timestamps()

        # Smooth out the noise
        smooth_signal_x = self.gaussian_filter(signal_x)
        smooth_signal_y = self.gaussian_filter(signal_y)
        smooth_signal_z = self.gaussian_filter(signal_z)

        render_trio(signal_x, signal_y, signal_z, timestamps)
        render_trio(smooth_signal_x, smooth_signal_y, smooth_signal_z, timestamps)

        # g is the difference between the smoothed version and the actual version
        g = [ [], [], [] ]
        delta_g = [ [], [], [] ]
        delta_g[0] = np.subtract(signal_x, smooth_signal_x).tolist()
        delta_g[1] = np.subtract(signal_y, smooth_signal_y).tolist()
        delta_g[2] = np.subtract(signal_z, smooth_signal_z).tolist()
        g[0] = signal_x #np.subtract(signal_x, smooth_signal_x).tolist()
        g[1] = signal_y #np.subtract(signal_y, smooth_signal_y).tolist()
        g[2] = signal_z #np.subtract(signal_z, smooth_signal_z).tolist()
        dgt = utilities.diff(timestamps)

        theta = [ [], [], [] ]
        delta_theta = [ [], [], [] ]
        for component in [0, 1, 2]:
            sum_of_consecutives = np.add(g[component][:-1], g[component][1:])
            # The 2 is for the integration - and 10e9 for the nanosecond
            dx_0 = np.divide(sum_of_consecutives, 2 * 1000000000)
            num_0 = np.multiply(dx_0, dgt)
            theta[component] = [0]
            theta[component].extend(np.cumsum(num_0))

            sum_of_delta_consecutives = np.add(delta_g[component][:-1], delta_g[component][1:])
            dx_0 = np.divide(sum_of_delta_consecutives, 2 * 1000000000)
            num_0 = np.multiply(dx_0, dgt)
            delta_theta[component] = [0]
            delta_theta[component].extend(np.cumsum(num_0))

        # UNKNOWNS
        focal_length = 1080.0
        gyro_delay = 0
        gyro_drift = (0, 0, 0)
        shutter_duration = 0

        parts = self.mp4.split("/")
        pickle_file_name = parts[-1].split(".")[0]
        pickle_full_path = "%s/%s.pickle" % ("/".join(parts[:-1]), pickle_file_name)
        print("Pickle file = %s" % pickle_full_path)

        import pickle
        videoObj = None
        if not os.path.exists(pickle_full_path):
            print("Pickle file not found - generating it")
            videoObj = GyroVideo(self.mp4)
            videoObj.read_video()
            fp = open(pickle_full_path, "w")
            pickle.dump(videoObj, fp)
            fp.close()
        else:
            fp = open(pickle_full_path, "r")
            videoObj = pickle.load(fp)
            fp.close()

        print "Calibrating parameters"
        print "=====================+"

        parameters = np.asarray([1080.0, 
                                    0.0,
                                    0.0, 0.0, 0.0,
                                    0.0])

        import scipy.optimize
        result = scipy.optimize.minimize(self.calcErrorAcrossVideoObjective, parameters, (videoObj, theta, timestamps), 'Nelder-Mead', tol=0.001)
        print result

        focal_length = result['x'][0]
        gyro_delay = result['x'][1]
        gyro_drift = ( result['x'][2], result['x'][3], result['x'][4] )
        shutter_duration = result['x'][5]

        print "Focal length = %f" % focal_length
        print "Gyro delay   = %f" % gyro_delay
        print "Gyro drift   = (%f, %f, %f)" % gyro_drift
        print "Shutter duration= %f" % shutter_duration

        # Smooth out the delta_theta values - they must be fluctuating like crazy

        smooth_delta_x = self.gaussian_filter(delta_theta[0], 128, 16)
        smooth_delta_y = self.gaussian_filter(delta_theta[1], 128, 16)
        smooth_delta_z = self.gaussian_filter(delta_theta[2], 128, 16)
        return (delta_theta, timestamps, focal_length, gyro_delay, gyro_drift, shutter_duration)
        #return ( (smooth_delta_x, smooth_delta_y, smooth_delta_z), timestamps, focal_length, gyro_delay, gyro_drift, shutter_duration)



def stabilize_video(mp4, csv):
    calib_obj = CalibrateGyroStabilize(mp4, csv)
    delta_theta, timestamps, focal_length, gyro_delay, gyro_drift, shutter_duration = calib_obj.calibrate()

    # Now start reading the frames
    vidcap = cv2.VideoCapture(mp4)

    frameCount = 0
    success, frame = vidcap.read()
    previous_timestamp = 0
    while success:
        print "Processing frame %d" % frameCount
        # Timestamp in nanoseconds
        current_timestamp = vidcap.get(0) * 1000 * 1000
        print "    timestamp = %s ns" % current_timestamp
        rot, prev, current = fetch_closest_trio(delta_theta[0], delta_theta[1], delta_theta[2], timestamps, current_timestamp)

        rot = accumulateRotation(frame, delta_theta[0], delta_theta[1], delta_theta[2], timestamps, previous_timestamp, prev, focal_length, gyro_delay, gyro_drift)

        #print "    rotation: %f, %f, %f" % (rot[0] * 180 / math.pi, 
        #                                    rot[1] * 180 / math.pi,
        #                                    rot[2] * 180 / math.pi)
        cv2.imwrite('/tmp/frame%04d.png' % frameCount, frame)
        cv2.imwrite('/tmp/rotated%04d.png' % frameCount, rot)
        frameCount += 1
        previous_timestamp = prev
        success, frame = vidcap.read()

        if frameCount == MAX_FRAMES:
            break

if __name__ == "__main__":
    if len(sys.argv) == 1:
        raise Exception("Please pass the path to an mp4 file")

    mp4path = sys.argv[1]
    csvpath = mp4path.replace('.mp4', '.gyro.csv')

    print("MP4 = %s" % mp4path)
    print("CSV = %s" % csvpath)

    stabilize_video(mp4path, csvpath)
