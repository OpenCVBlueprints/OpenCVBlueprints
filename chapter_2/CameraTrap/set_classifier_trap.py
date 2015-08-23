#!/usr/bin/env python

from __future__ import print_function

import argparse
import sys
import time

from CVBackwardCompat import cv2

import CameraCommander


def main():

    parser = argparse.ArgumentParser(
        description='This script detects objects using an '
                    'attached webcam. When it detects '
                    'objects that match a given cascade '
                    'file, it captures photos on an attached '
                    'gPhoto2-compatible photo camera.')

    parser.add_argument(
        '--debug', type=bool, default=False,
        help='print debugging information')

    parser.add_argument(
        '--cam-index', type=int, default=-1,
        help='device index for the detection camera '
             '(default=0)')
    parser.add_argument(
        '--width', type=int, default=320,
        help='capture width for the detection camera '
             '(default=320)')
    parser.add_argument(
        '--height', type=int, default=240,
        help='capture height for the detection camera '
             '(default=240)')
    parser.add_argument(
        '--detection-interval', type=float, default=0.25,
        help='interval between detection frames, in seconds '
             '(default=0.25)')

    parser.add_argument(
        '--cascade-file', type=str, required=True,
        help='path to cascade file that classifier will use '
             'to detect objects in scene')
    parser.add_argument(
        '--scale-factor', type=float, default=1.05,
        help='relative difference in scale between '
             'iterations of multi-scale classification '
             '(default=1.05)')
    parser.add_argument(
        '--min-neighbors', type=int, default=8,
        help='minimum number of overlapping objects that '
             'classifier must detect in order to trigger '
             'classification event (default=8)')
    parser.add_argument(
        '--min-object-width', type=int, default=40,
        help='minimum width of each detected object'
             '(default=40)')
    parser.add_argument(
        '--min-object-height', type=int, default=40,
        help='minimum height of each detected object'
             '(default=40)')

    parser.add_argument(
        '--photo-count', type=int, default=1,
        help='number of photo frames per motion event '
             '(default=1)')
    parser.add_argument(
        '--photo-interval', type=float, default=3.0,
        help='interval between photo frames, in seconds '
             '(default=3.0)')
    parser.add_argument(
        '--photo-ev-step', type=float, default=None,
        help='exposure step between photo frames, in EV. If '
             'this is specified, --photo-interval is ignored '
             'and --photo-count refers to the length of an '
             'exposure bracketing sequence, not a time-lapse '
             'sequence.')

    args = parser.parse_args()

    debug = args.debug

    cam_index = args.cam_index
    w, h = args.width, args.height
    detection_interval = args.detection_interval

    classifier = cv2.CascadeClassifier(args.cascade_file)
    if classifier.empty():
        print('Failed to read cascade file: %s' %
              args.cascade_file, file=sys.stderr)
        return

    scale_factor = args.scale_factor
    min_neighbors = args.min_neighbors
    min_size = (args.min_object_width, args.min_object_height)

    photo_count = args.photo_count
    photo_interval = args.photo_interval
    photo_ev_step = args.photo_ev_step

    cap = cv2.VideoCapture(cam_index)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, w)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, h)

    bgr = None
    gray = None

    cc = CameraCommander.CameraCommander()

    while True:
        time.sleep(detection_interval)
        success, bgr = cap.read(bgr)
        if success:
            gray = cv2.cvtColor(bgr, cv2.COLOR_BGR2GRAY, gray)
            gray = cv2.equalizeHist(gray, gray)
            objects = classifier.detectMultiScale(
                gray, scaleFactor=scale_factor,
                minNeighbors=min_neighbors,
                minSize=min_size)
            num_objects = len(objects)
            if debug:
                print('num_objects=%d' % num_objects)
            if num_objects > 0 and not cc.capturing:
                if photo_ev_step is not None:
                    cc.capture_exposure_bracket(
                        photo_ev_step, photo_count)
                else:
                    cc.capture_time_lapse(
                        photo_interval, photo_count)


if __name__ == '__main__':
    main()
