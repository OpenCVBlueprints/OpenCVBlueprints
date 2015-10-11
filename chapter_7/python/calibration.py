#!/usr/bin/python

import sys
import numpy

class InvalidFileException(Exception):
    pass

class CalibrationWorker(object):
    """
    This module does something
    """
    def __init__(self):
        pass

    def something(self):
        return True

class CalibrationParameters(object):
    def __init__(self):
        self.f = 0.0                # The focal length
        self.td = 0.0               # The gyroscope delay
        self.gb = (0.0, 0.0, 0.0)   # The gyroscope bias
        self.ts = 0.0               # The rolling shutter duration

class GyroscopeDataFile(object):
    """
    """
    def __init__(self, filepath):
        self.filepath = filepath
        self.omega = {}

    def _get_file_object(self):
        return open(self.filepath)

    def parse(self):
        with self._get_file_object() as fp:
            firstline = fp.readline().strip()

            if not firstline == 'utk':
                raise InvalidFileException("The first line isn't valid")

            for line in fp.readlines():
                line = line.strip()
                parts = line.split(',')

                print("%s: %s, %s, %s" % (int(parts[3]), float(parts[0]), float(parts[1]), float(parts[2])))
                self.omega[int(parts[3])] = (float(parts[0]), float(parts[1]), float(parts[2]))

    def fetch_approximate_omega(self, timestamp):
        """
        Returns the closest match for a given timestamp
        """
        if timestamp in self.omega:
            return self.omega[timestamp]

        i = 0
        sorted_keys = sorted(self.omega.keys())
        for ts in sorted_keys:
            if ts > timestamp:
                break

            i += 1

        # We're looking for the ith and the i+1th timestamp
        t_previous = sorted_keys[i]
        t_current = sorted_keys[i+1]
        dt = float(t_current - t_previous)

        slope = (timestamp - t_previous) / dt

        new_x = self.omega[t_previous][0] * (1-slope) + self.omega[t_current][0]*slope
        new_y = self.omega[t_previous][1] * (1-slope) + self.omega[t_current][1]*slope
        new_z = self.omega[t_previous][2] * (1-slope) + self.omega[t_current][2]*slope

        return (new_x, new_y, new_z)

    def get_signal(self, index):
        keys = sorted(self.omega.keys())
        return [self.omega[k][index] for k in keys]

    def get_signal_x(self):
        return self.get_signal(0)

    def get_signal_y(self):
        return self.get_signal(1)

    def get_signal_z(self):
        return self.get_signal(2)

    def get_timestamps(self):
        return sorted(self.omega.keys())


if __name__ == '__main__':
    g = GyroscopeDataFile('/work/blueprints/c7/data/mockgyro.csv')
    g.parse()

    print g.fetch_approximate_omega(0)
    print g.fetch_approximate_omega(50)
    print g.fetch_approximate_omega(100)

    g = GyroscopeDataFile('/work/blueprints/c7/data/VID_20150405_005248gyro.csv')
    g.parse()
    print g.fetch_approximate_omega(0)
    print g.fetch_approximate_omega(1)
    print g.fetch_approximate_omega(5065918)
