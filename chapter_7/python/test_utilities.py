#!/usr/bin/python

import unittest
import utilities

class GaussianFilterTest(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_kernelGenerator(self):
        # Normalized
        kernel = utilities.get_gaussian_kernel(1, 0, 0)
        self.assertEquals(kernel, [1])

        # Un normalized
        kernel = utilities.get_gaussian_kernel(1, -1, 1)
        self.assertAlmostEqual(kernel[0], 0.21194155761708544)
        self.assertAlmostEqual(kernel[1], 0.5761168847658291)
        self.assertAlmostEqual(kernel[2], 0.21194155761708544)

    def test_filterOutput(self):
        output = utilities.gaussian_filter(range(0, 1001))

        self.assertAlmostEqual(output[135], 135.000)
        self.assertAlmostEqual(output[690], 690.000)
        self.assertAlmostEqual(output[1000], 487.0011735)

    def test_diff(self):
        output = utilities.diff([0, 1, 2, 3, 4])

        self.assertEqual(output.tolist(), [1, 1, 1, 1])

if __name__ == '__main__':
    unittest.main()
