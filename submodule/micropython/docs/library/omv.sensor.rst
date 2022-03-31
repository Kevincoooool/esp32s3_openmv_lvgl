:mod:`sensor` --- camera sensor
===============================

.. module:: sensor
   :synopsis: camera sensor

The ``sensor`` module is used for taking pictures.

Example usage::

    import sensor

    # Setup camera.
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.skip_frames()

    # Take pictures.
    while(True):
        sensor.snapshot()

Functions
---------

.. function:: sensor.reset()

   Initializes the camera sensor.

.. function:: sensor.sleep(enable)

   Puts the camera to sleep if enable is True. Otherwise, wakes it back up.

.. function:: sensor.shutdown(enable)

   Puts the camera into a lower power mode than sleep (but the camera must be reset on being woken up).

.. function:: sensor.flush()

   Copies whatever was in the frame buffer to the IDE. You should call this
   method to display the last image your OpenMV Cam takes if it's not running
   a script with an infinite loop. Note that you'll need to add a delay time
   of about a second after your script finishes for the IDE to grab the image
   from your camera. Otherwise, this method will have no effect.

.. function:: sensor.snapshot()

   Takes a picture using the camera and returns an ``image`` object.

   The OpenMV Cam has two memory areas for images. The classical stack/heap
   area used for normal MicroPython processing can store small images within
   it's heap. However, the MicroPython heap is only about ~100 KB which is not
   enough to store larger images. So, your OpenMV Cam has a secondary frame
   buffer memory area that stores images taken by `sensor.snapshot()`. Images
   are stored on the bottom of this memory area. Any memory that's left
   over is then available for use by the frame buffer stack which your OpenMV
   Cam's firmware uses to hold large temporary data structures for image
   processing algorithms.

   If you need room to hold multiple frames you may "steal" frame buffer space
   by calling `sensor.alloc_extra_fb()`.

   If `sensor.set_auto_rotation()` is enabled this method will return a new
   already rotated `image` object.

   .. note::

      `sensor.snapshot()` may apply cropping parameters to fit the snapshot in the available
      RAM the pixformat, framesize, windowing, and framebuffers. The cropping parameters will be applied
      to maintain the aspect ratio and will stay until `sensor.set_framesize()` or `sensor.set_windowing()` are called.

.. function:: sensor.skip_frames([n, time])

   Takes ``n`` number of snapshots to let the camera image stabilize after
   changing camera settings. ``n`` is passed as normal argument, e.g.
   ``skip_frames(10)`` to skip 10 frames. You should call this function after
   changing camera settings.

   Alternatively, you can pass the keyword argument ``time`` to skip frames
   for some number of milliseconds, e.g. ``skip_frames(time = 2000)`` to skip
   frames for 2000 milliseconds.

   If neither ``n`` nor ``time`` is specified this method skips frames for
   300 milliseconds.

   If both are specified this method skips ``n`` number of frames but will
   timeout after ``time`` milliseconds.

   .. note::

      `sensor.snapshot()` may apply cropping parameters to fit the snapshot in the available
      RAM given the pixformat, framesize, windowing, and framebuffers. The cropping parameters will be applied
      to maintain the aspect ratio and will stay until `sensor.set_framesize()` or `sensor.set_windowing()` are called.

.. function:: sensor.width()

   Returns the sensor resolution width.

.. function:: sensor.height()

   Returns the sensor resolution height.

.. function:: sensor.get_fb()

   (Get Frame Buffer) Returns the image object returned by a previous call of
   `sensor.snapshot()`. If `sensor.snapshot()` had not been called before
   then ``None`` is returned.

.. function:: sensor.get_id()

   Returns the camera module ID.

      * `sensor.OV9650`: First gen OpenMV Cam sensor - never released.
      * `sensor.OV2640`: Second gen OpenMV Cam sensor - never released.
      * `sensor.OV5640`: High-res OpenMV Cam H7 sensor.
      * `sensor.OV7725`: Rolling shutter sensor module.
      * `sensor.OV7690`: OpenMV Cam Micro sensor module.
      * `sensor.MT9V034`: Global shutter sensor module.
      * `sensor.MT9M114`: New Rolling shutter sensor module.
      * `sensor.LEPTON`: Lepton1/2/3 sensor module.
      * `sensor.HM01B0`: Arduino Portenta H7 sensor module.
      * `sensor.GC2145`: Arduino Nicla Vision H7 sensor module.

.. function:: sensor.alloc_extra_fb(width, height, pixformat)

   Allocates another frame buffer for image storage from the frame buffer stack
   and returns an ``image`` object of ``width``, ``height``, and ``pixformat``.

   You may call this function as many times as you like as long as there's
   memory available to allocate any number of extra frame buffers.

   If ``pixformat`` is a number >= 4 then this will allocate a JPEG image. You
   can then do `image.bytearray()` to get byte level read/write access to the JPEG image.

   .. note::

      Creating secondary images normally requires creating them on the heap which
      has a limited amount of RAM... but, also gets fragmented making it hard to
      grab a large contigous memory array to store an image in. With this method
      you are able to allocate a very large memory array for an image instantly
      by taking space away from our frame buffer stack memory which we use for
      computer vision algorithms. That said, this also means you'll run out of
      memory more easily if you try to execute more memory intensive machine
      vision algorithms like `image.find_apriltags`.

.. function:: sensor.dealloc_extra_fb()

   Deallocates the last previously allocated extra frame buffer. Extra frame
   buffers are stored in a stack like structure.

   .. note::

      Your OpenMV Cam has two memory areas. First, you have your classical
      .data/.bss/heap/stack memory area. The .data/.bss/heap regions are
      fixed by firmware. The stack then grows down until it hits the heap.
      Next, frame buffers are stored in a secondary memory region. Memory is
      liad out with the main frame buffer on the bottom and the frame buffer
      stack on the top. When `sensor.snapshot()` is called it fills the frame bufer
      from the bottom. The frame buffer stack is then able to use whatever is
      left over. This memory allocation method is extremely efficent for computer
      vision on microcontrollers.

.. function:: sensor.set_pixformat(pixformat)

   Sets the pixel format for the camera module.

      * `sensor.GRAYSCALE`: 8-bits per pixel.
      * `sensor.RGB565`: 16-bits per pixel.
      * `sensor.BAYER`: 8-bits per pixel bayer pattern.
      * `sensor.JPEG`: Compressed JPEG data. Only for the OV2640/OV5640.

   If you are trying to take JPEG images with the OV2640 or OV5640 camera modules at high
   resolutions you should set the pixformat to `sensor.JPEG`. You can control the image
   quality then with `sensor.set_quality()`.

.. function:: sensor.get_pixformat()

   Returns the pixformat for the camera module.

.. function:: sensor.set_framesize(framesize)

   Sets the frame size for the camera module.

      * `sensor.QQCIF`: 88x72
      * `sensor.QCIF`: 176x144
      * `sensor.CIF`: 352x288
      * `sensor.QQSIF`: 88x60
      * `sensor.QSIF`: 176x120
      * `sensor.SIF`: 352x240
      * `sensor.QQQQVGA`: 40x30
      * `sensor.QQQVGA`: 80x60
      * `sensor.QQVGA`: 160x120
      * `sensor.QVGA`: 320x240
      * `sensor.VGA`: 640x480
      * `sensor.HQQQVGA`: 60x40
      * `sensor.HQQVGA`: 120x80
      * `sensor.HQVGA`: 240x160
      * `sensor.B64X32`: 64x32 (for use with `image.find_displacement()`)
      * `sensor.B64X64`: 64x64 (for use with `image.find_displacement()`)
      * `sensor.B128X64`: 128x64 (for use with `image.find_displacement()`)
      * `sensor.B128X128`: 128x128 (for use with `image.find_displacement()`)
      * `sensor.B320X320`: 320x320 (for the HM01B0)
      * `sensor.LCD`: 128x160 (for use with the lcd shield)
      * `sensor.QQVGA2`: 128x160 (for use with the lcd shield)
      * `sensor.WVGA`: 720x480 (for the MT9V034)
      * `sensor.WVGA2`:752x480 (for the MT9V034)
      * `sensor.SVGA`: 800x600 (only for the OV2640/OV5640 sensor)
      * `sensor.XGA`: 1024x768 (only for the OV2640/OV5640 sensor)
      * `sensor.SXGA`: 1280x1024 (only for the OV2640/OV5640 sensor)
      * `sensor.UXGA`: 1600x1200 (only for the OV2640/OV5640 sensor)
      * `sensor.HD`: 1280x720 (only for the OV2640/OV5640 sensor)
      * `sensor.FHD`: 1920x1080 (only for the OV5640 sensor)
      * `sensor.QHD`: 2560x1440 (only for the OV5640 sensor)
      * `sensor.QXGA`: 2048x1536 (only for the OV5640 sensor)
      * `sensor.WQXGA`: 2560x1600 (only for the OV5640 sensor)
      * `sensor.WQXGA2`: 2592x1944 (only for the OV5640 sensor)

.. function:: sensor.get_framesize()

   Returns the frame size for the camera module.

.. function:: sensor.set_framerate(rate)

   Sets the frame rate in hz on the HM01B0. May be 15, 30, 60, or 120 Hz.

.. function:: sensor.get_framerate()

   Returns the frame rate in hz on the HM01B0.

.. function:: sensor.set_windowing(roi)

   Sets the resolution of the camera to a sub resolution inside of the current
   resolution. For example, setting the resolution to `sensor.VGA` and then
   the windowing to (120, 140, 200, 200) sets `sensor.snapshot()` to capture
   the 200x200 center pixels of the VGA resolution outputted by the camera
   sensor. You can use windowing to get custom resolutions. Also, when using
   windowing on a larger resolution you effectively are digital zooming.

   ``roi`` is a rect tuple (x, y, w, h). However, you may just pass (w, h) and
   the ``roi`` will be centered on the frame. You may also pass roi not in parens.

   This function will automatically handle cropping the passed roi to the framesize.

.. function:: sensor.get_windowing()

   Returns the ``roi`` tuple (x, y, w, h) previously set with `sensor.set_windowing()`.

.. function:: sensor.set_gainceiling(gainceiling)

   Set the camera image gainceiling. 2, 4, 8, 16, 32, 64, or 128.

.. function:: sensor.set_contrast(constrast)

   Set the camera image contrast. -3 to +3.

.. function:: sensor.set_brightness(brightness)

   Set the camera image brightness. -3 to +3.

.. function:: sensor.set_saturation(saturation)

   Set the camera image saturation. -3 to +3.

.. function:: sensor.set_quality(quality)

   Set the camera image JPEG compression quality. 0 - 100.

   .. note::

      Only for the OV2640/OV5640 cameras.

.. function:: sensor.set_colorbar(enable)

   Turns color bar mode on (True) or off (False). Defaults to off.

.. function:: sensor.set_auto_gain(enable, [gain_db=-1, [gain_db_ceiling]])

   ``enable`` turns auto gain control on (True) or off (False).
   The camera will startup with auto gain control on.

   If ``enable`` is False you may set a fixed gain in decibels with ``gain_db``.

   If ``enable`` is True you may set the maximum gain ceiling in decibels with
   ``gain_db_ceiling`` for the automatic gain control algorithm.

   .. note::

      You need to turn off white balance too if you want to track colors.

.. function:: sensor.get_gain_db()

   Returns the current camera gain value in decibels (float).

.. function:: sensor.set_auto_exposure(enable, [exposure_us])

   ``enable`` turns auto exposure control on (True) or off (False).
   The camera will startup with auto exposure control on.

   If ``enable`` is False you may set a fixed exposure time in microseconds
   with ``exposure_us``.

   .. note::

      Camera auto exposure algorithms are pretty conservative about how much
      they adjust the exposure value by and will generally avoid changing the
      exposure value by much. Instead, they change the gain value alot of deal
      with changing lighting.

.. function:: sensor.get_exposure_us()

   Returns the current camera exposure value in microseconds (int).

.. function:: sensor.set_auto_whitebal(enable, [rgb_gain_db])

   ``enable`` turns auto white balance on (True) or off (False).
   The camera will startup with auto white balance on.

   If ``enable`` is False you may set a fixed gain in decibels for the red, green,
   and blue channels respectively with ``rgb_gain_db``.

   .. note::

      You need to turn off gain control too if you want to track colors.

.. function:: sensor.get_rgb_gain_db()

   Returns a tuple with the current camera red, green, and blue gain values in
   decibels ((float, float, float)).

.. function:: sensor.set_hmirror(enable)

   Turns horizontal mirror mode on (True) or off (False). Defaults to off.

.. function:: sensor.get_hmirror()

   Returns if horizontal mirror mode is enabled.

.. function:: sensor.set_vflip(enable)

   Turns vertical flip mode on (True) or off (False). Defaults to off.

.. function:: sensor.get_vflip()

   Returns if vertical flip mode is enabled.

.. function:: sensor.set_transpose(enable)

   Turns transpose mode on (True) or off (False). Defaults to off.

      * vflip=False, hmirror=False, transpose=False -> 0 degree rotation
      * vflip=True,  hmirror=False, transpose=True  -> 90 degree rotation
      * vflip=True,  hmirror=True,  transpose=False -> 180 degree rotation
      * vflip=False, hmirror=True,  transpose=True  -> 270 degree rotation

.. function:: sensor.get_transpose()

   Returns if transpose mode is enabled.

.. function:: sensor.set_auto_rotation(enable)

   Turns auto rotation mode on (True) or off (False). Defaults to off.

   .. note::

      This function only works when the OpenMV Cam has an `imu` installed and is enabled automatically.

.. function:: sensor.get_auto_rotation()

   Returns if auto rotation mode is enabled.

   .. note::

      This function only works when the OpenMV Cam has an `imu` installed and is enabled automatically.

.. function:: sensor.set_framebuffers(count)

   Sets the number of frame buffers used to receive image data. By default your OpenMV Cam will
   automatically try to allocate the maximum number of frame buffers it can possibly allocate
   without using more than 1/2 of the available frame buffer RAM at the time of allocation to
   ensure the best performance. Automatic reallocation of frame buffers occurs whenever you
   call `sensor.set_pixformat()`, `sensor.set_framesize()`, and `sensor.set_windowing()`.

   `sensor.snapshot()` will automatically handle switching active frame buffers in the background.
   From your code's perspective there is only ever 1 active frame buffer even though there might
   be more than 1 frame buffer on the system and another frame buffer reciving data in the background.

   If count is:

      1 - Single Buffer Mode (you may also pass `sensor.SINGLE_BUFFER`)
          In single buffer mode your OpenMV Cam will allocate one frame buffer for receiving images.
          When you call `sensor.snapshot()` that framebuffer will be used to receive the image and
          the camera driver will continue to run. In the advent you call `sensor.snapshot()` again
          before the first line of the next frame is received your code will execute at the frame rate
          of the camera. Otherwise, the image will be dropped.

      2 - Double Buffer Mode (you may also pass `sensor.DOUBLE_BUFFER`)
          In double buffer mode your OpenMV Cam will allocate two frame buffers for receiving images.
          When you call `sensor.snapshot()` one framebuffer will be used to receive the image and
          the camera driver will continue to run. When the next frame is received it will be stored
          in the other frame bufer. In the advent you call `sensor.snapshot()` again
          before the first line of the next frame after is received your code will execute at the frame rate
          of the camera. Otherwise, the image will be dropped.

      3 - Triple Buffer Mode (you may also pass `sensor.TRIPLE_BUFFER`)
          In triple buffer mode your OpenMV Cam will allocate three buffers for receiving images.
          In this mode there is always a frame buffer to store the received image to in the background
          resulting in the highest performance and lowest latency for reading the latest received frame.
          No frames are ever dropped in this mode. The next frame read by `sensor.snapshot()` is the
          last captured frame by the sensor driver (e.g. if you are reading slower than the camera
          frame rate then the older frame in the possible frames available is skipped).

   Regarding the reallocation above, triple buffering is tried first, then double buffering, and if
   these both fail to fit in 1/2 of the available frame buffer RAM then single buffer mode is used.

   You may pass a value of 4 or greater to put the sensor driver into video FIFO mode where received
   images are stored in a frame buffer FIFO with ``count`` buffers. This is useful for video recording
   to an SD card which may randomly block your code from writing data when the SD card is performing
   house-keeping tasks like pre-erasing blocks to write data to.

   .. note::

      On frame drop (no buffers available to receive the next frame) all frame buffers are automatically
      cleared except the active frame buffer. This is done to ensure `sensor.snapshot()` returns current
      frames and not frames from long ago.

   Fun fact, you can pass a value of 100 or so on OpenMV Cam's with SDRAM for a huge video fifo. If
   you then call snapshot slower than the camera frame rate (by adding `pyb.delay()`) you'll get
   slow-mo effects in OpenMV IDE. However, you will also see the above policy effect of resetting
   the frame buffer on a frame drop to ensure that frames do not get too old. If you want to record
   slow-mo video just record video normally to the SD card and then play the video back on a desktop
   machine slower than it was recorded.

.. function:: sensor.get_framebuffers()

   Returns the current number of frame buffers allocated.

.. function:: sensor.set_lens_correction(enable, radi, coef)

   ``enable`` True to enable and False to disable (bool).
   ``radi`` integer radius of pixels to correct (int).
   ``coef`` power of correction (int).

.. function:: sensor.set_vsync_callback(cb)

   Registers callback ``cb`` to be executed (in interrupt context) whenever the camera module
   generates a new frame (but, before the frame is received).

   ``cb`` takes one argument and is passed the current state of the vsync pin after changing.

.. function:: sensor.set_frame_callback(cb)

   Registers callback ``cb`` to be executed (in interrupt context) whenever the camera module
   generates a new frame and the frame is ready to be read via `sensor.snapshot()`.

   ``cb`` takes no arguments.

   Use this to get an interrupt to schedule reading a frame later with `micropython.schedule()`.

.. function:: sensor.get_frame_available()

   Returns True if a frame is available to read by calling `sensor.snapshot()`.

.. function:: sensor.ioctl(...)

   Executes a sensor specific method:

   * `sensor.IOCTL_SET_READOUT_WINDOW` - Pass this enum followed by a rect tuple (x, y, w, h) or a size tuple (w, h).
      * This IOCTL allows you to control the readout window of the camera sensor which dramatically improves the frame rate at the cost of field-of-view.
      * If you pass a rect tuple (x, y, w, h) the readout window will be positoned on that rect tuple. The rect tuple's x/y position will be adjusted so the size w/h fits. Additionally, the size w/h will be adjusted to not be smaller than the ``framesize``.
      * If you pass a size tuple (w, h) the readout window will be centered given the w/h. Additionally, the size w/h will be adjusted to not be smaller than the ``framesize``.
      * This IOCTL is extremely helpful for increasing the frame rate on higher resolution cameras like the OV2640/OV5640.
   * `sensor.IOCTL_GET_READOUT_WINDOW` - Pass this enum for `sensor.ioctl` to return the current readout window rect tuple (x, y, w, h). By default this is (0, 0, maximum_camera_sensor_pixel_width, maximum_camera_sensor_pixel_height).
   * `sensor.IOCTL_SET_TRIGGERED_MODE` - Pass this enum followed by True or False set triggered mode for the MT9V034 sensor.
   * `sensor.IOCTL_GET_TRIGGERED_MODE` - Pass this enum for `sensor.ioctl` to return the current triggered mode state.
   * `sensor.IOCTL_TRIGGER_AUTO_FOCUS` - Pass this enum for `sensor.ioctl` to trigger auto focus on the OV5640 FPC camera module.
   * `sensor.IOCTL_PAUSE_AUTO_FOCUS` - Pass this enum for `sensor.ioctl` to pause auto focus (after triggering) on the OV5640 FPC camera module.
   * `sensor.IOCTL_RESET_AUTO_FOCUS` - Pass this enum for `sensor.ioctl` to reset auto focus (after triggering) on the OV5640 FPC camera module.
   * `sensor.IOCTL_WAIT_ON_AUTO_FOCUS` - Pass this enum for `sensor.ioctl` to wait for auto focus (after triggering) to finish on the OV5640 FPC camera module. You may pass a second argument of the timeout in milliseconds. The default is 5000 ms.
   * `sensor.IOCTL_LEPTON_GET_WIDTH` - Pass this enum to get the FLIR Lepton image width in pixels.
   * `sensor.IOCTL_LEPTON_GET_HEIGHT` - Pass this enum to get the FLIR Lepton image height in pixels.
   * `sensor.IOCTL_LEPTON_GET_RADIOMETRY` - Pass this enum to get the FLIR Lepton type (radiometric or not).
   * `sensor.IOCTL_LEPTON_GET_REFRESH` - Pass this enum to get the FLIR Lepton refresh rate in hertz.
   * `sensor.IOCTL_LEPTON_GET_RESOLUTION` - Pass this enum to get the FLIR Lepton ADC resolution in bits.
   * `sensor.IOCTL_LEPTON_RUN_COMMAND` - Pass this enum to execute a FLIR Lepton SDK command. You need to pass an additional 16-bit value after the enum as the command to execute.
   * `sensor.IOCTL_LEPTON_SET_ATTRIBUTE` - Pass this enum to set a FLIR Lepton SDK attribute.
      * The first argument is the 16-bit attribute ID to set (set the FLIR Lepton SDK).
      * The second argument is a MicroPython byte array of bytes to write (should be a multiple of 16-bits). Create the byte array using `struct` following the FLIR Lepton SDK.
   * `sensor.IOCTL_LEPTON_GET_ATTRIBUTE` - Pass this enum to get a FLIR Lepton SDK attribute.
      * The first argument is the 16-bit attribute ID to set (set the FLIR Lepton SDK).
      * Returns a MicroPython byte array of the attribute. Use `struct` to deserialize the byte array following the FLIR Lepton SDK.
   * `sensor.IOCTL_LEPTON_GET_FPA_TEMPERATURE` - Pass this enum to get the FLIR Lepton FPA Temp in celsius.
   * `sensor.IOCTL_LEPTON_GET_AUX_TEMPERATURE` - Pass this enum to get the FLIR Lepton AUX Temp in celsius.
   * `sensor.IOCTL_LEPTON_SET_MEASUREMENT_MODE` - Pass this followed by True or False to turn off automatic gain control on the FLIR Lepton and force it to output an image where each pixel value represents an exact temperature value in celsius. A second True enables high temperature mode enabling measurements up to 500C on the Lepton 3.5, False is the default low temperature mode.
   * `sensor.IOCTL_LEPTON_GET_MEASUREMENT_MODE` - Pass this to get a tuple for (measurment-mode-enabled, high-temp-enabled).
   * `sensor.IOCTL_LEPTON_SET_MEASUREMENT_RANGE` - Pass this when measurement mode is enabled to set the temperature range in celsius for the mapping operation. The temperature image returned by the FLIR Lepton will then be clamped between these min and max values and then scaled to values between 0 to 255. To map a pixel value back to a temperature (on a grayscale image) do: ((pixel * (max_temp_in_celsius - min_temp_in_celsius)) / 255.0) + min_temp_in_celsius.
      * The first arugment should be the min temperature in celsius.
      * The second argument should be the max temperature in celsius. If the arguments are reversed the library will automatically swap them for you.
   * `sensor.IOCTL_LEPTON_GET_MEASUREMENT_RANGE` - Pass this to return the sorted (min, max) 2 value temperature range tuple. The default is -10C to 40C if not set yet.
   * `sensor.IOCTL_HIMAX_MD_ENABLE` - Pass this enum followed by ``True``/``False`` to enable/disable motion detection on the HM01B0. You should also enable the I/O pin (PC15 on the Arduino Portenta) attached the HM01B0 motion detection line to receive an interrupt.
   * `sensor.IOCTL_HIMAX_MD_CLEAR` - Pass this enum to clear the motion detection interrupt on the HM01B0.
   * `sensor.IOCTL_HIMAX_MD_WINDOW` - Pass this enum followed by (x1, y1, x2, y2) to set the motion detection window on the HM01B0.
   * `sensor.IOCTL_HIMAX_MD_THRESHOLD` - Pass this enum followed by a threshold value (0-255) to set the motion detection threshold on the HM01B0.
   * `sensor.IOCTL_HIMAX_OSC_ENABLE` - Pass this enum followed by ``True``/``False`` to enable/disable the oscillator HM01B0 to save power.

.. function:: sensor.set_color_palette(palette)

   Sets the color palette to use for FLIR Lepton grayscale to RGB565 conversion.

.. function:: sensor.get_color_palette()

   Returns the current color palette setting. Defaults to `sensor.PALETTE_RAINBOW`.

.. function:: sensor.__write_reg(address, value)

   Write ``value`` (int) to camera register at ``address`` (int).

   .. note:: See the camera data sheet for register info.

.. function:: sensor.__read_reg(address)

   Read camera register at ``address`` (int).

   .. note:: See the camera data sheet for register info.

Constants
---------

.. data:: sensor.BINARY

   BINARY (bitmap) pixel format. Each pixel is 1-bit.

   This format is usful for mask storage. Can be used with `image.Image()` and
   `sensor.alloc_extra_fb()`.

.. data:: sensor.GRAYSCALE

   GRAYSCALE pixel format (Y from YUV422). Each pixel is 8-bits, 1-byte.

   All of our computer vision algorithms run faster on grayscale images than
   RGB565 images.

.. data:: sensor.RGB565

   RGB565 pixel format. Each pixel is 16-bits, 2-bytes. 5-bits are used for red,
   6-bits are used for green, and 5-bits are used for blue.

   All of our computer vision algorithms run slower on RGB565 images than
   grayscale images.

.. data:: sensor.BAYER

   RAW BAYER image pixel format. If you try to make the frame size too big
   to fit in the frame buffer your OpenMV Cam will set the pixel format
   to BAYER so that you can capture images but no image processing methods
   will be operational.

.. data:: sensor.JPEG

   JPEG mode. The camera module outputs compressed jpeg images.
   Use `sensor.set_quality()` to control the jpeg quality.
   Only works for the OV2640/OV5640 cameras.

.. data:: sensor.OV2640

   `sensor.get_id()` returns this for the OV2640 camera.

.. data:: sensor.OV5640

   `sensor.get_id()` returns this for the OV5640 camera.

.. data:: sensor.OV7690

   `sensor.get_id()` returns this for the OV7690 camera.

.. data:: sensor.OV7725

   `sensor.get_id()` returns this for the OV7725 camera.

.. data:: sensor.OV9650

   `sensor.get_id()` returns this for the OV9650 camera.

.. data:: sensor.MT9M114

   `sensor.get_id()` returns this for the MT9M114 camera.

.. data:: sensor.MT9V034

   `sensor.get_id()` returns this for the MT9V034 camera.

.. data:: sensor.LEPTON

   `sensor.get_id()` returns this for the LEPTON1/2/3 cameras.

.. data:: sensor.HM01B0

   `sensor.get_id()` returns this for the HM01B0 camera.

.. data:: sensor.GC2145

   `sensor.get_id()` returns this for the GC2145 camera.

.. data:: sensor.QQCIF

   88x72 resolution for the camera sensor.

.. data:: sensor.QCIF

   176x144 resolution for the camera sensor.

.. data:: sensor.CIF

   352x288 resolution for the camera sensor.

.. data:: sensor.QQSIF

   88x60 resolution for the camera sensor.

.. data:: sensor.QSIF

   176x120 resolution for the camera sensor.

.. data:: sensor.SIF

   352x240 resolution for the camera sensor.

.. data:: sensor.QQQQVGA

   40x30 resolution for the camera sensor.

.. data:: sensor.QQQVGA

   80x60 resolution for the camera sensor.

.. data:: sensor.QQVGA

   160x120 resolution for the camera sensor.

.. data:: sensor.QVGA

   320x240 resolution for the camera sensor.

.. data:: sensor.VGA

   640x480 resolution for the camera sensor.

.. data:: sensor.HQQQVGA

   60x40 resolution for the camera sensor.

.. data:: sensor.HQQVGA

   120x80 resolution for the camera sensor.

.. data:: sensor.HQVGA

   240x160 resolution for the camera sensor.

.. data:: sensor.B64X32

   64x32 resolution for the camera sensor.

   For use with `image.find_displacement()` and any other FFT based algorithm.

.. data:: sensor.B64X64

   64x64 resolution for the camera sensor.

   For use with `image.find_displacement()` and any other FFT based algorithm.

.. data:: sensor.B128X64

   128x64 resolution for the camera sensor.

   For use with `image.find_displacement()` and any other FFT based algorithm.

.. data:: sensor.B128X128

   128x128 resolution for the camera sensor.

   For use with `image.find_displacement()` and any other FFT based algorithm.

.. data:: sensor.B320X320

   320x320 resolution for the HM01B0 camera sensor.

.. data:: sensor.LCD

   128x160 resolution for the camera sensor (for use with the lcd shield).

.. data:: sensor.QQVGA2

   128x160 resolution for the camera sensor (for use with the lcd shield).

.. data:: sensor.WVGA

   720x480 resolution for the MT9V034 camera sensor.

.. data:: sensor.WVGA2

   752x480 resolution for the MT9V034 camera sensor.

.. data:: sensor.SVGA

   800x600 resolution for the camera sensor. Only works for the OV2640/OV5640 cameras.

.. data:: sensor.XGA

   1024x768 resolution for the camera sensor. Only works for the OV2640/OV5640 cameras.

.. data:: sensor.SXGA

   1280x1024 resolution for the camera sensor. Only works for the OV2640/OV5640 cameras.

.. data:: sensor.UXGA

   1600x1200 resolution for the camera sensor. Only works for the OV2640/OV5640 cameras.

.. data:: sensor.HD

   1280x720 resolution for the camera sensor. Only works for the OV2640/OV5640 cameras.

.. data:: sensor.FHD

   1920x1080 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: sensor.QHD

   2560x1440 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: sensor.QXGA

   2048x1536 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: sensor.WQXGA

   2560x1600 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: sensor.WQXGA2

   2592x1944 resolution for the camera sensor. Only works for the OV5640 camera.

.. data:: sensor.PALETTE_RAINBOW

   Default OpenMV Cam color palette for thermal images using a smooth color wheel.

.. data:: sensor.PALETTE_IRONBOW

   Makes images look like the FLIR Lepton thermal images using a very non-linear color palette.

.. data:: sensor.IOCTL_SET_READOUT_WINDOW

   Lets you set the readout window for the OV5640.

.. data:: sensor.IOCTL_GET_READOUT_WINDOW

   Lets you get the readout window for the OV5640.

.. data:: sensor.IOCTL_SET_TRIGGERED_MODE

   Lets you set the triggered mode for the MT9V034.

.. data:: sensor.IOCTL_GET_TRIGGERED_MODE

   Lets you get the triggered mode for the MT9V034.

.. data:: sensor.IOCTL_TRIGGER_AUTO_FOCUS

   Used to trigger auto focus for the OV5640 FPC camera module.

.. data:: sensor.IOCTL_PAUSE_AUTO_FOCUS

   Used to pause auto focus (while running) for the OV5640 FPC camera module.

.. data:: sensor.IOCTL_RESET_AUTO_FOCUS

   Used to reset auto focus back to the default for the OV5640 FPC camera module.

.. data:: sensor.IOCTL_WAIT_ON_AUTO_FOCUS

   Used to wait on auto focus to finish after being triggered for the OV5640 FPC camera module.

.. data:: sensor.IOCTL_LEPTON_GET_WIDTH

   Lets you get the FLIR Lepton image resolution width in pixels.

.. data:: sensor.IOCTL_LEPTON_GET_HEIGHT

   Lets you get the FLIR Lepton image resolution height in pixels.

.. data:: sensor.IOCTL_LEPTON_GET_RADIOMETRY

   Lets you get the FLIR Lepton type (radiometric or not).

.. data:: sensor.IOCTL_LEPTON_GET_REFRESH

   Lets you get the FLIR Lepton refresh rate in hertz.

.. data:: sensor.IOCTL_LEPTON_GET_RESOLUTION

   Lets you get the FLIR Lepton ADC resolution in bits.

.. data:: sensor.IOCTL_LEPTON_RUN_COMMAND

   Executes a 16-bit command given the FLIR Lepton SDK.

.. data:: sensor.IOCTL_LEPTON_SET_ATTRIBUTE

   Sets a FLIR Lepton Attribute given the FLIR Lepton SDK.

.. data:: sensor.IOCTL_LEPTON_GET_ATTRIBUTE

   Gets a FLIR Lepton Attribute given the FLIR Lepton SDK.

.. data:: sensor.IOCTL_LEPTON_GET_FPA_TEMPERATURE

   Gets the FLIR Lepton FPA temp in celsius.

.. data:: sensor.IOCTL_LEPTON_GET_AUX_TEMPERATURE

   Gets the FLIR Lepton AUX temp in celsius.

.. data:: sensor.IOCTL_LEPTON_SET_MEASUREMENT_MODE

   Lets you set the FLIR Lepton driver into a mode where you can get a valid temperature value per pixel. See `sensor.ioctl()` for more information.

.. data:: sensor.IOCTL_LEPTON_GET_MEASUREMENT_MODE

   Lets you get if measurement mode is enabled or not for the FLIR Lepton sensor. See `sensor.ioctl()` for more information.

.. data:: sensor.IOCTL_LEPTON_SET_MEASUREMENT_RANGE

   Lets you set the temperature range you want to map pixels in the image to when in measurement mode. See `sensor.ioctl()` for more information.

.. data:: sensor.IOCTL_LEPTON_GET_MEASUREMENT_RANGE

   Lets you get the temperature range used for measurement mode. See `sensor.ioctl()` for more information.

.. data:: sensor.IOCTL_HIMAX_MD_ENABLE

   Lets you control the motion detection interrupt on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: sensor.IOCTL_HIMAX_MD_CLEAR

   Lets you control the motion detection interrupt on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: sensor.IOCTL_HIMAX_MD_WINDOW

   Lets you control the motion detection interrupt on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: sensor.IOCTL_HIMAX_MD_THRESHOLD

   Lets you control the motion detection interrupt on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: sensor.IOCTL_HIMAX_OSC_ENABLE

   Lets you control the internal oscillator on the HM01B0. See `sensor.ioctl()` for more information.

.. data:: sensor.SINGLE_BUFFER

   Pass to `sensor.set_framebuffers()` to set single buffer mode (1 buffer).

.. data:: sensor.DOUBLE_BUFFER

   Pass to `sensor.set_framebuffers()` to set double buffer mode (2 buffers).

.. data:: sensor.TRIPLE_BUFFER

   Pass to `sensor.set_framebuffers()` to set triple buffer mode (3 buffers).

.. data:: sensor.VIDEO_FIFO

   Pass to `sensor.set_framebuffers()` to set video FIFO mode (4 buffers).
