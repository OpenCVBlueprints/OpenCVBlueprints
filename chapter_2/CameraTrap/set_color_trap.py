#!/usr/bin/env python

from __future__ import print_function

import argparse
import sys
import time

from CVBackwardCompat import cv2

import CameraCommander


def main():

    parser = argparse.ArgumentParser(
        description='This script detects colors using an '
                    'attached webcam. When it detects colors '
                    'that match the histogram of a reference '
                    'image, it captures photos on an '
                    'attached gPhoto2-compatible photo '
                    'camera.')

    parser.add_argument(
        '--debug', type=bool, default=False,
        help='print debugging information')

    parser.add_argument(
        '--cam-index', type=int, default=-1,
        help='device index for detection camera '
             '(default=0)')
    parser.add_argument(
        '--width', type=int, default=320,
        help='capture width for detection camera '
             '(default=320)')
    parser.add_argument(
        '--height', type=int, default=240,
        help='capture height for detection camera '
             '(default=240)')
    parser.add_argument(
        '--detection-interval', type=float, default=0.25,
        help='interval between detection frames, in seconds '
             '(default=0.25)')

    parser.add_argument(
        '--reference-image', type=str, required=True,
        help='path to reference image, whose colors will be '
             'detected in scene')
    parser.add_argument(
        '--min-similarity', type=float, default=0.02,
        help='similarity score that histogram comparator '
             'must find in order to trigger similarity event '
             '(default=0.02, valid_range=[0.0, 1.0])')

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

    reference_image = cv2.imread(args.reference_image,
                                 cv2.IMREAD_COLOR)
    if reference_image is None:
        print('Failed to read reference image: %s' %
              args.reference_image, file=sys.stderr)
        return

    min_similarity = args.min_similarity

    photo_count = args.photo_count
    photo_interval = args.photo_interval
    photo_ev_step = args.photo_ev_step

    cap = cv2.VideoCapture(cam_index)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, w)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, h)

    bgr = None

    channels = range(3)
    hist_size = [256] * 3
    ranges = [0, 255] * 3

    def create_normalized_hist(image, hist=None):
        hist = cv2.calcHist(
            [image], channels, None, hist_size, ranges, hist)
        return cv2.normalize(
            hist, hist, norm_type=cv2.NORM_L1)

    reference_hist = create_normalized_hist(reference_image)
    query_hist = None

    cc = CameraCommander.CameraCommander()

    while True:
        time.sleep(detection_interval)
        success, bgr = cap.read(bgr)
        if success:
            query_hist = create_normalized_hist(
                bgr, query_hist)
            similarity = cv2.compareHist(
                reference_hist, query_hist,
                cv2.HISTCMP_INTERSECT)
            if debug:
                print('similarity=%f' % similarity)
            if similarity >= min_similarity and not cc.capturing:
                if photo_ev_step is not None:
                    cc.capture_exposure_bracket(
                        photo_ev_step, photo_count)
                else:
                    cc.capture_time_lapse(
                        photo_interval, photo_count)


if __name__ == '__main__':
    main()
