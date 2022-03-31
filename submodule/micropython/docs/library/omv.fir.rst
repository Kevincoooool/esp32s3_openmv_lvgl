:mod:`fir` --- thermal sensor driver (fir == far infrared)
==========================================================

.. module:: fir
   :synopsis: thermal sensor driver (fir == far infrared)

The ``fir`` module is used for controlling the thermal sensors.

Example usage::

    import sensor, fir

    # Setup camera.
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.skip_frames()
    fir.init()

    # Show image.
    while(True):
        img = sensor.snapshot()
        ta, ir, to_min, to_max = fir.read_ir()
        fir.draw_ir(image, ir)
        print("====================")
        print("Ambient temperature: %0.2f" % ta)
        print("Min temperature seen: %0.2f" % to_min)
        print("Max temperature seen: %0.2f" % to_max)

Functions
---------

.. function:: fir.init([type=-1, [refresh, [resolution]]])

   Initializes an attached thermopile shield using I/O pins P4 and P5 (and P0, P1, P2, P3 for `fir.FIR_LEPTON`)

   ``type`` indicates the type of thermopile shield:

      * `fir.FIR_NONE`: 0 pixels.
      * `fir.FIR_SHIELD`: 16x4 pixels.
      * `fir.FIR_MLX90621`: 16x4 pixels.
      * `fir.FIR_MLX90640`: 32x24 pixels.
      * `fir.FIR_MLX90641`: 16x12 pixels.
      * `fir.FIR_AMG8833`: 8x8 pixels.
      * `fir.FIR_LEPTON`: 80x60 pixels (FLIR Lepton 1.x/2.x) or 160x120 pixels (FLIR Lepton 3.x)

   By default type is ``-1`` which will cause `fir.init()` to automatically scan and initialize an
   attached thermal sensor based on the I2C address. Note that `fir.FIR_MLX90640` and
   `fir.FIR_MLX90641` have the same I2C address so you must pass `fir.FIR_MLX90641` to type
   to initialize it specifically.

   `fir.FIR_LEPTON` on the OpenMV Cam Pure Thermal this uses internal I/O pins and does not use P0-P5.

   ``refresh`` is the thermopile sensor power-of-2 refresh rate in Hz:

      * `fir.FIR_NONE`: N/A
      * `fir.FIR_SHIELD`: Defaults to 64 Hz. Can be 1 Hz, 2 Hz, 4 Hz, 8 Hz, 16 Hz, 32 Hz, 64 Hz, 128 Hz, 256 Hz, or 512 Hz. Note that a higher refresh rate lowers the accuracy and vice-versa.
      * `fir.FIR_MLX90621`: Defaults to 64 Hz. Can be 1 Hz, 2 Hz, 4 Hz, 8 Hz, 16 Hz, 32 Hz, 64 Hz, 128 Hz, 256 Hz, or 512 Hz. Note that a higher refresh rate lowers the accuracy and vice-versa.
      * `fir.FIR_MLX90640`: Defaults to 32 Hz. Can be 1 Hz, 2 Hz, 4 Hz, 8 Hz, 16 Hz, 32 Hz, or 64 Hz. Note that a higher refresh rate lowers the accuracy and vice-versa.
      * `fir.FIR_MLX90641`: Defaults to 32 Hz. Can be 1 Hz, 2 Hz, 4 Hz, 8 Hz, 16 Hz, 32 Hz, or 64 Hz. Note that a higher refresh rate lowers the accuracy and vice-versa.
      * `fir.FIR_AMG8833`: 10 Hz
      * `fir.FIR_LEPTON`: 9 Hz (really 8.7 Hz).

   ``resolution`` is the thermopile sensor measurement resolution:

      * `fir.FIR_NONE`: N/A
      * `fir.FIR_SHIELD`: Defaults to 18-bits. Can be 15-bits, 16-bits, 17-bits, or 18-bits. Note that a higher resolution lowers the maximum temperature range and vice-versa.
      * `fir.FIR_MLX90621`: Defaults to 18-bits. Can be 15-bits, 16-bits, 17-bits, or 18-bits. Note that a higher resolution lowers the maximum temperature range and vice-versa.
      * `fir.FIR_MLX90640`: Defaults to 19-bits. Can be 16-bits, 17-bits, 18-bits, or 19-bits. Note that a higher resolution lowers the maximum temperature range and vice-versa.
      * `fir.FIR_MLX90641`: Defaults to 19-bits. Can be 16-bits, 17-bits, 18-bits, or 19-bits. Note that a higher resolution lowers the maximum temperature range and vice-versa.
      * `fir.FIR_AMG8833`: 12-bits.
      * `fir.FIR_LEPTON`: 14-bits.

   For the `fir.FIR_SHIELD` and `fir.FIR_MLX90621`:

      * 15-bits -> Max of ~950C.
      * 16-bits -> Max of ~750C.
      * 17-bits -> Max of ~600C.
      * 18-bits -> Max of ~450C.

   For the `fir.FIR_MLX90640` and `fir.FIR_MLX90641`:

      * 16-bits -> Max of ~750C.
      * 17-bits -> Max of ~600C.
      * 18-bits -> Max of ~450C.
      * 19-bits -> Max of ~300C.

   For the `fir.FIR_AMG8833`:

      * Max of ~80C.

   For the `fir.FIR_LEPTON`:

      * Max of ~140C (can be up to 400C-450C in low-gain mode).

   .. note::

      For `fir.FIR_LEPTON` mode this driver implements triple buffering to receive the FLIR Lepton
      image. This uses 28.125 KB of RAM for the FLIR Lepton 1.x/2.x and 112.5 KB of RAM for the
      FLIR Lepton 3.x. Triple buffering ensures that reading an image with `fir.read_ir()` and
      `fir.snapshot()` never block. For all other sensors the I2C bus is accessed to read the image.

.. function:: fir.deinit()

   Deinitializes the thermal sensor freeing up resources.

.. function:: fir.width()

   Returns the width (horizontal resolution) of the thermal sensor in-use:

      * `fir.FIR_NONE`: 0 pixels.
      * `fir.FIR_SHIELD`: 16 pixels.
      * `fir.FIR_MLX90621`: 16 pixels.
      * `fir.FIR_MLX90640`: 32 pixels.
      * `fir.FIR_MLX90641`: 16 pixels.
      * `fir.FIR_AMG8833`: 8 pixels.
      * `fir.FIR_LEPTON`: 80 pixels (FLIR Lepton 1.x/2.x) or 160 pixels (FLIR Lepton 3.x).

.. function:: fir.height()

   Returns the height (vertical resolution) of the thermal sensor in-use:

      * `fir.FIR_NONE`: 0 pixels.
      * `fir.FIR_SHIELD`: 4 pixels.
      * `fir.FIR_MLX90621`: 4 pixels.
      * `fir.FIR_MLX90640`: 24 pixels.
      * `fir.FIR_MLX90641`: 12 pixels.
      * `fir.FIR_AMG8833`: 8 pixels.
      * `fir.FIR_LEPTON`: 60 pixels (FLIR Lepton 1.x/2.x) or 120 pixels (FLIR Lepton 3.x).

.. function:: fir.type()

   Returns the type of the thermal sensor in-use:

      * `fir.FIR_NONE`
      * `fir.FIR_SHIELD`
      * `fir.FIR_MLX90621`
      * `fir.FIR_MLX90640`
      * `fir.FIR_MLX90641`
      * `fir.FIR_AMG8833`
      * `fir.FIR_LEPTON`

.. function:: fir.refresh()

   Returns the current refresh rate set during `fir.init()` call.

.. function:: fir.resolution()

   Returns the current resolution set during the `fir.init()` call.

.. function:: fir.radiometric()

   Returns if the thermal sensor reports accurate temperature readings (True or False). If False
   this means that the thermal sensor reports relative temperature readings based on its ambient
   temperature which may not be very accurate.

.. function:: fir.register_vsync_cb(cb)

   For the `fir.FIR_LEPTON` mode only on the OpenMV Cam Pure Thermal.

   Registers callback ``cb`` to be executed (in interrupt context) whenever the FLIR Lepton
   generates a new frame (but, before the frame is received).

   This nomially triggers at 9 Hz.

   ``cb`` takes no arguments.

.. function:: fir.register_frame_cb(cb)

   For the `fir.FIR_LEPTON` mode only on the OpenMV Cam Pure Thermal.

   Registers callback ``cb`` to be executed (in interrupt context) whenever the FLIR Lepton
   generates a new frame and the frame is ready to be read via `fir.read_ir()` or `fir.snapshot()`.

   This nomially triggers at 9 Hz.

   ``cb`` takes no arguments.

   Use this to get an interrupt to schedule reading a frame later with `micropython.schedule()`.

.. function:: fir.get_frame_available()

   Returns True if a frame is available to read by calling `fir.read_ir()` or `fir.snapshot()`.

.. function:: fir.trigger_ffc([timeout=-1])

   For the `fir.FIR_LEPTON` mode only.

   Triggers the Flat-Field-Correction process on your FLIR Lepton which calibrates the thermal
   image. This process happens automatically with the sensor. However, you may call this function
   to force the process to happen.

   ``timeout`` if not -1 then how many milliseconds to wait for FFC to complete.

.. function:: fir.read_ta()

   Returns the ambient temperature (i.e. sensor temperature).

   Example::

      ta = fir.read_ta()

   The value returned is a float that represents the temperature in celsius.

.. function:: fir.read_ir([hmirror=False, [vflip=False, [transpose=False, [timeout=-1]]]])

   Returns a tuple containing the ambient temperature (i.e. sensor temperature),
   the temperature list (width * height), the minimum temperature seen, and
   the maximum temperature seen.

   ``hmirror`` if set to True horizontally mirrors the ``ir`` array.

   ``vflip`` if set to True vertically flips the ``ir`` array.

   ``transpose`` if set to True transposes the ``ir`` array.

   ``timeout`` if not -1 then how many milliseconds to wait for the new frame.

   If you want to rotate an image by multiples of 90 degrees pass the following::

      * vflip=False, hmirror=False, transpose=False -> 0 degree rotation
      * vflip=True,  hmirror=False, transpose=True  -> 90 degree rotation
      * vflip=True,  hmirror=True,  transpose=False -> 180 degree rotation
      * vflip=False, hmirror=True,  transpose=True  -> 270 degree rotation

   Example::

      ta, ir, to_min, to_max = fir.read_ir()

   The values returned are floats that represent the temperature in celsius.

   .. note::

      ``ir`` is a (width * height) list of floats (4-bytes each).

.. function:: fir.draw_ir(image, ir, [x, [y, [x_scale=1.0, [y_scale=1.0, [roi=None, [rgb_channel=-1, [alpha=128, [color_palette=fir.PALETTE_RAINBOW, [alpha_palette=-1, [hint=0, [x_size=None, [y_size=None, [scale=(ir_min, ir_max)]]]]]]]]]]]]])

   Draws an ``ir`` array on ``image`` whose top-left corner starts at location x, y. You may either pass x, y
   separately, as a tuple (x, y), or not at all. This method automatically handles rendering the image passed
   into the correct pixel format for the destination image while also handling clipping seamlessly.

   ``x_scale`` controls how much the drawn image is scaled by in the x direction (float). If this
   value is negative the image will be flipped horizontally.

   ``y_scale`` controls how much the drawn image is scaled by in the y direction (float). If this
   value is negative the image will be flipped vertically.

   ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
   allows you to extract just the pixels in the ROI to scale and draw on the destination image.

   ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
   and to render onto the destination image. For example, if you pass ``rgb_channel=1`` this will
   extract the green channel of the source RGB565 image and draw that in grayscale on the
   destination image.

   ``alpha`` controls how much of the source image to blend into the destination image. A value of
   256 draws an opaque source image while a value lower than 256 produces a blend between the source
   and destination image. 0 results in no modification to the destination image.

   ``color_palette`` if not ``-1`` can be `sensor.PALETTE_RAINBOW`, `sensor.PALETTE_IRONBOW`, or
   a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
   whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

   ``alpha_palette`` if not ``-1`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
   palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
   level allowing you to precisely control the alpha value of pixels based on their grayscale value.
   A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
   more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

   ``hint`` can be a logical OR of the flags:

      * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
      * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
      * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
      * `image.CENTER`: Center the image image being draw on (x, y).
      * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
      * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
      * `image.BLACK_BACKGROUND`: Assume the destination image is black. This speeds up drawing.

   ``x_size`` may be passed if ``x_scale`` is not passed to specify the size of the image to draw
   and ``x_scale`` will automatically be determined passed on the input image size. If neither
   ``y_scale`` or ``y_size`` are specified then ``y_scale`` internally will be set to be equal to
   ``x_size`` to maintain the aspect-ratio.

   ``y_size`` may be passed if ``y_scale`` is not passed to specify the size of the image to draw
   and ``y_scale`` will automatically be determined passed on the input image size. If neither
   ``x_scale`` or ``x_size`` are specified then ``x_scale`` internally will be set to be equal to
   ``y_size`` to maintain the aspect-ratio.

   ``scale`` is a two value tuple which controls the min and max temperature (in celsius) to scale
   the ``ir`` image. By default it's equal to the image ``ir`` min and ``ir`` max.

   If x/y are not specified the image will be centered in the field of view. If x_scale/y_scale or
   x_size/y_size are not specified the ``ir`` array will be scaled to fit on the ``image``.

   .. note::

      To handle a transposed ``ir`` array `read_ir` remembers if it was called with ``transposed``
      ``True``. This is then passed to ``draw_ir`` internally. However, you may pass a 3-value tuple
      (w, h, ir) as the ``ir`` array instead to use `draw_ir` to draw any floating point array with
      width ``w`` and height ``h``.

.. function:: fir.snapshot([hmirror=False, [vflip=False, [transpose=False, [x_scale=1.0, [y_scale=1.0, [roi=None, [rgb_channel=-1, [alpha=128, [color_palette=fir.PALETTE_RAINBOW, [alpha_palette=None, [hint=0, [x_size=None, [y_size=None, [scale=(ir_min, ir_max), [pixformat=fir.PIXFORMAT_RGB565, [copy_to_fb=False, [timeout=-1]]]]]]]]]]]]]]]])

   Works like `sensor.snapshot()` and returns an `image` object that is either
   `fir.PIXFORMAT_GRAYSCALE` (grayscale) or `fir.PIXFORMAT_RGB565` (color). If ``copy_to_fb`` is False then
   the new image is allocated on the MicroPython heap. However, the MicroPython heap is limited
   and may not have space to store the new image if exhausted. Instead, set ``copy_to_fb`` to
   True to set the frame buffer to the new image making this function work just like `sensor.snapshot()`.

   ``hmirror`` if set to True horizontally mirrors the new image.

   ``vflip`` if set to True vertically flips the new image.

   ``transpose`` if set to True transposes the new image.

   If you want to rotate an image by multiples of 90 degrees pass the following::

      * vflip=False, hmirror=False, transpose=False -> 0 degree rotation
      * vflip=True,  hmirror=False, transpose=True  -> 90 degree rotation
      * vflip=True,  hmirror=True,  transpose=False -> 180 degree rotation
      * vflip=False, hmirror=True,  transpose=True  -> 270 degree rotation

   ``x_scale`` controls how much the drawn image is scaled by in the x direction (float). If this
   value is negative the image will be flipped horizontally.

   ``y_scale`` controls how much the drawn image is scaled by in the y direction (float). If this
   value is negative the image will be flipped vertically.

   ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
   allows you to extract just the pixels in the ROI to scale and draw on the destination image.

   ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
   and to render onto the destination image. For example, if you pass ``rgb_channel=1`` this will
   extract the green channel of the source RGB565 image and draw that in grayscale on the
   destination image.

   ``alpha`` controls how much of the source image to blend into the destination image. A value of
   256 draws an opaque source image while a value lower than 256 produces a blend between the source
   and destination image. 0 results in no modification to the destination image.

   ``color_palette`` if not ``-1`` can be `sensor.PALETTE_RAINBOW`, `sensor.PALETTE_IRONBOW`, or
   a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
   whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

   ``alpha_palette`` if not ``-1`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
   palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
   level allowing you to precisely control the alpha value of pixels based on their grayscale value.
   A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
   more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

   ``hint`` can be a logical OR of the flags:

      * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
      * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
      * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
      * `image.CENTER`: Center the image image being draw on (x, y).
      * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
      * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
      * `image.BLACK_BACKGROUND`: Assume the destination image is black. This speeds up drawing.

   ``x_size`` may be passed if ``x_scale`` is not passed to specify the size of the image to draw
   and ``x_scale`` will automatically be determined passed on the input image size. If neither
   ``y_scale`` or ``y_size`` are specified then ``y_scale`` internally will be set to be equal to
   ``x_size`` to maintain the aspect-ratio.

   ``y_size`` may be passed if ``y_scale`` is not passed to specify the size of the image to draw
   and ``y_scale`` will automatically be determined passed on the input image size. If neither
   ``x_scale`` or ``x_size`` are specified then ``x_scale`` internally will be set to be equal to
   ``y_size`` to maintain the aspect-ratio.

   ``scale`` is a two value tuple which controls the min and max temperature (in celsius) to scale
   the ``ir`` image. By default it's equal to the image ``ir`` min and ``ir`` max.

   ``pixformat`` if specified controls the final image pixel format.

   ``copy_to_fb`` may also be another image object if you want to replace that image object's memory
   buffer, type, width, and height with new image data.

   ``timeout`` if not -1 then how many milliseconds to wait for the new frame.

   .. note::

      Any use of ``copy_to_fb`` invalidates the previous image object it overwrites. Do not use
      any references to previous image objects anymore it overwrites. Either for an image object
      referencing the frame buffer, frame buffer stack, or an image on the MicroPython heap.

   Returns an image object.

Constants
---------

.. data:: fir.FIR_NONE

   No FIR sensor type.

.. data:: fir.FIR_SHIELD

   The OpenMV Cam Thermopile Shield Type (MLX90621).

.. data:: fir.FIR_MLX90621

   FIR_MLX90621 FIR sensor.

.. data:: fir.FIR_MLX90640

   FIR_MLX90640 FIR sensor.

.. data:: fir.FIR_MLX90641

   FIR_MLX90640 FIR sensor.

.. data:: fir.FIR_AMG8833

   FIR_AMG8833 FIR sensor.

.. data:: fir.FIR_LEPTON

   FIR_LEPTON FIR sensor.

.. data:: fir.PALETTE_RAINBOW

   Rainbow color palette for `fir.draw_ir()` and `fir.snapshot()`.

.. data:: fir.PALETTE_IRONBOW

   Ironbow color palette for `fir.draw_ir()` and `fir.snapshot()`.

.. data:: fir.PIXFORMAT_GRAYSCALE

   GRAYSCALE pixformat for `fir.snapshot()`.

.. data:: fir.PIXFORMAT_RGB565

   RGB565 pixformat for `fir.snapshot()`.
