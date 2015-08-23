import os
import subprocess


class CameraCommander(object):


    def __init__(self, logPath=os.devnull):
        self._logFile = open(logPath, 'w')
        self._capProc = None
        self.unmount_cameras()


    def __del__(self):
        self._logFile.close()


    def _open_proc(self, command):
        return subprocess.Popen(
            command, shell=True, stdout=self._logFile,
            stderr=self._logFile)


    def unmount_cameras(self):
        proc = self._open_proc('./unmount_cameras.sh')
        proc.wait()


    def capture_image(self):
        self.stop_capture()
        self._capProc = self._open_proc('gphoto2 --capture-image')


    def capture_time_lapse(self, interval, frames=0):
        self.stop_capture()
        if frames <= 0:
            # Capture an indefinite number of images.
            command = 'gphoto2 --capture-image -I %d' % interval
        else:
            command = 'gphoto2 --capture-image -I %d -F %d' %\
                (interval, frames)
        self._capProc = self._open_proc(command)


    def capture_exposure_bracket(self, ev_step=1.0, frames=3):
        self.stop_capture()
        self._capProc = self._open_proc(
            './capture_exposure_bracket.sh -s %d -f %d' %\
                (int(ev_step * 1000), frames))


    def stop_capture(self):
        if self._capProc is not None:
            if self._capProc.poll() is None:
                # The process is currently running but might finish
                # before the next function call.
                try:
                    self._capProc.terminate()
                except:
                    # The process already finished.
                    pass
            self._capProc = None


    def wait_capture(self):
        if self._capProc is not None:
            self._capProc.wait()
            self._capProc = None


    @property
    def capturing(self):
        if self._capProc is None:
            return False
        elif self._capProc.poll() is None:
            return True
        else:
            self._capProc = None
            return False
