#!/usr/bin/env python

import CameraCommander


def main():

    cc = CameraCommander.CameraCommander('test_camera_commands.log')

    cc.capture_image()
    print('Capturing image...')
    cc.wait_capture()
    print('Done')

    cc.capture_time_lapse(3, 2)
    print('Capturing 2 images at time interval of 3 seconds...')
    cc.wait_capture()
    print('Done')

    cc.capture_exposure_bracket(1.0, 3)
    print('Capturing 3 images at exposure interval of 1.0 EV...')
    cc.wait_capture()
    print('Done')


if __name__ == '__main__':
    main()
