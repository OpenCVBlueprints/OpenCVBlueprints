#!/usr/bin/env python

from CVBackwardCompat import cv2


def main():

    ldr_images = [
        cv2.imread('media/PlasmaWink_0.jpg'),
        cv2.imread('media/PlasmaWink_1.jpg')]

    hdr_processor = cv2.createMergeMertens()
    hdr_image = hdr_processor.process(ldr_images) * 255
    cv2.imwrite('media/PlasmaWink_HDR.jpg', hdr_image)


if __name__ == '__main__':
    main()
