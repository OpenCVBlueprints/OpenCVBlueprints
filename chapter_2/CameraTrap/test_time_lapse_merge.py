#!/usr/bin/env python

from CVBackwardCompat import cv2


def main():

    num_input_files = 56
    input_filename_pattern = 'media/JosephineChair_%02d.jpg'
    output_filename = 'media/JosephineChair_TimeLapse.avi'
    fourcc = cv2.VideoWriter_fourcc('M', 'J', 'P', 'G')
    fps = 60.0
    writer = None

    for i in range(num_input_files):
        input_filename = input_filename_pattern % i
        image = cv2.imread(input_filename)
        if writer is None:
            is_color = (len(image.shape) > 2)
            h, w = image.shape[:2]
            writer = cv2.VideoWriter(
                output_filename, fourcc, fps, (w, h), is_color)
        writer.write(image)


if __name__ == '__main__':
    main()
