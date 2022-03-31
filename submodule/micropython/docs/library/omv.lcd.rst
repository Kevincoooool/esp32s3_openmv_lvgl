:mod:`lcd` --- lcd driver
=========================

.. module:: lcd
   :synopsis: lcd driver

The ``lcd`` module is used for driving SPI LCDs, 24-bit parallel LCDs, MIPI DSI LCDs, HDMI output, and Display Port output.

Example usage for driving the 128x160 LCD shield::

    import sensor, lcd

    # Setup camera.
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.LCD)
    sensor.skip_frames()
    lcd.init()

    # Show image.
    while(True):
        lcd.display(sensor.snapshot())

Functions
---------

.. function:: lcd.init([type=lcd.LCD_SHIELD, [width=128, [height=160, [framesize=lcd.FWVGA, [refresh=60, [triple_buffer=False, [bgr=False]]]]]]])

   Initializes an attached lcd module.

   ``type`` indicates how the lcd module should be initialized:

      * `lcd.LCD_NONE`: Do nothing.
      * `lcd.LCD_SHIELD`: Initialize a SPI LCD display. Uses pins P0, P2, P3, P6, P7, and P8.
      * `lcd.LCD_DISPLAY`: Initialize the LCD module driving a high resolution display.
      * `lcd.LCD_DISPLAY_WITH_HDMI`: Initialize dirivng a high res display and HDMI output too.
      * `lcd.LCD_DISPLAY_ONLY_HDMI`: Initialize only driving the HDMI output.

    ``width`` SPI LCD width in `lcd.LCD_SHIELD` mode. By default this is 128 to match the OpenMV
    128x160 LCD shield.

    ``height`` SPI LCD height in `lcd.LCD_SHIELD` mode. By default this is 160 to match the OpenMV
    128x160 LCD shield.

    ``framesize`` One of the standard supported resolutions in LCD_DISPLAY* modes.

    ``refresh`` Sets the LCD refresh rate in hertz. In particular, this controls the SPI LCD shield
    clock and high resolution display clock. If you are experiencing bandwidth issues driving the
    external display you may lower this to lower the memory bandwidth requirements. Alternatively,
    you can raise this if all system components allow for faster screen updates.

    ``triple_buffer`` If True then makes updates to the screen non-blocking in `lcd.LCD_SHIELD`
    mode at the cost of 3X the display RAM. In LCD_DISPLAY* modes triple_buffer is always on.

    ``bgr`` set to True in `lcd.LCD_SHIELD` to swap the red and blue channels.

.. function:: lcd.deinit()

   Deinitializes the lcd module, internal/external hardware, and I/O pins.

.. function:: lcd.width()

   Returns the width of the screen that was set during `lcd.init()`.

.. function:: lcd.height()

   Returns the height of the screen that was set during `lcd.init()`.

.. function:: lcd.type()

   Returns the type of the screen that was set during `lcd.init()`.

.. function:: lcd.triple_buffer()

   Returns if triple buffering is enabled that was set during `lcd.init()`.

.. function:: lcd.bgr()

   Returns if the red and blue channels are swapped that was set during `lcd.init()`.

.. function:: lcd.framesize()

   Returns the framesize that was set during `lcd.init()`.

.. function:: lcd.refresh()

   Returns the refresh rate that was set during `lcd.init()`.

.. function:: lcd.set_backlight(value)

   Sets the lcd backlight dimming value. 0 (off) to 255 (on).

   In `lcd.LCD_SHIELD` mode this controls the DAC on P5 to provide the dimming value. If set to 0
   P5 is pulled low and if set to 255 P5 is unitialized assuming that the SPI LCD shield's backlight
   is by default always on.

   In LCD_DISPLAY* modes this controls a PWM signal to a standard backlight dimming circuit.

.. function:: lcd.get_backlight()

   Returns the lcd backlight dimming value.

.. function:: lcd.get_display_connected()

   In LCD_DISPLAY_*_HDMI modes returns if an external display is connected.

   This function can be called before `lcd.init()` so you can control how you init this module.

.. function:: lcd.register_hotplug_cb(callback)

   In LCD_DISPLAY_*_HDMI modes registers a ``callback`` function that be called whenever the state
   of an external display being connected changes. The new state will be passed as an argument.

   If you use this method do not call `lcd.get_display_connected()` anymore until the callback is
   disabled by pass ``None`` as the callback for this method.

.. function:: lcd.get_display_id_data()

   In LCD_DISPLAY_*_HDMI modes this function returns the external display EDID data as a bytes()
   object. Verifying the EDID headers, checksums, and concatenating all sections into one bytes()
   object is done for you. You may then parse this information by `following this guide <https://en.wikipedia.org/wiki/Extended_Display_Identification_Data>`__.

   This function can be called before `lcd.init()` so you can control how you init this module.

.. function:: lcd.send_frame(dst_addr, src_addr, bytes)

   In LCD_DISPLAY_*_HDMI modes this function sends a packet on the HDMI-CEC bus to ``dst_addr`` with
   source ``src_addr`` and data ``bytes``.

.. function:: lcd.receive_frame(dst_addr, timeout=1000)

   In LCD_DISPLAY_*_HDMI modes this function waits ``timeout`` milliseconds to receive an HDMI-CEC
   frame for address ``dst_addr``. Returns True if the received frame was for ``dst_addr`` and False
   if not. On timeout throws an `OSError` Exception.

.. function:: lcd.register_receive_cb(callback, dst_addr)

   In LCD_DISPLAY_*_HDMI modes registers a ``callback`` which will be called on reception of an
   HDMI-CEC frame. The callback will receive one argument of True or False if the HDMI-CEC frame
   was for ``dst_addr`` or not.

   If you use this method do not call `lcd.receive_frame()` anymore until the callback is
   disabled by pass ``None`` as the callback for this method.

.. function:: lcd.received_frame_src_addr()

   In LCD_DISPLAY_*_HDMI modes returns the received HDMI-CEC frame source address if
   `lcd.receive_frame()` or the callback in `lcd.register_receive_cb()` returned True.

   When a callback is enabled for the HDMI-CEC bus this method should not be called anymore except
   inside of the callback.

.. function:: lcd.received_frame_bytes()

   In LCD_DISPLAY_*_HDMI modes returns the received HDMI-CEC frame data payload as a bytes object
   if `lcd.receive_frame()` or the callback in `lcd.register_receive_cb()` returned True.

   When a callback is enabled for the HDMI-CEC bus this method should not be called anymore except
   inside of the callback.

.. function:: lcd.update_touch_points()

   In LCD_DISPLAY* modes this function reads the touch screen state and returns the number of touch
   points (0-5).

.. function:: lcd.register_touch_cb(callback)

   In LCD_DISPLAY* modes this function registers a callback which will receive the number of touch
   points (0-5) when a touch event happens.

   If you use this method do not call `lcd.update_touch_points()` anymore until the callback is
   disabled by pass ``None`` as the callback for this method.

.. function:: lcd.get_gesture()

   In LCD_DISPLAY* modes this returns the current touch gesture.

   This is one of LCD_GESTURE_*.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. function:: lcd.get_points()

   In LCD_DISPLAY* modes this returns the current number of touch points (0-5).

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. function:: lcd.get_point_flag(index)

   In LCD_DISPLAY* modes this returns the current touch point state of the point at ``index``.

   This is one of LCD_FLAG_*.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. function:: lcd.get_point_id(index)

   In LCD_DISPLAY* modes this returns the current touch point ``id`` of the point at ``index``.

   The touch point ``id`` is a numeric value that allows you to track a touch point as it may move
   around in list of touch points returned as points are added and removed.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. function:: lcd.get_point_x_position(index)

   In LCD_DISPLAY* modes this returns the current touch point x position of the point at ``index``.

   This is the x pixel position of the touch point on the screen.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. function:: lcd.get_point_y_position(index)

   In LCD_DISPLAY* modes this returns the current touch point y position of the point at ``index``.

   This is the y pixel position of the touch point on the screen.

   When a callback is enabled for the touch screen this method should not be called anymore except
   inside of the callback.

.. function:: lcd.display(image, [x=0, [y=0, [x_scale=1.0, [y_scale=1.0, [roi=None, [rgb_channel=-1, [alpha=256, [color_palette=None, [alpha_palette=None, [hint=0, [x_size=None, [y_size=None]]]]]]]]]]]])

   Displays an ``image`` whose top-left corner starts at location x, y. You may either pass x, y
   separately, as a tuple (x, y), or neither.

   ``x_scale`` controls how much the displayed image is scaled by in the x direction (float). If this
   value is negative the image will be flipped horizontally.

   ``y_scale`` controls how much the displayed image is scaled by in the y direction (float). If this
   value is negative the image will be flipped vertically.

   ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to display. This
   allows you to extract just the pixels in the ROI to scale.

   ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
   and to render on the display. For example, if you pass ``rgb_channel=1`` this will
   extract the green channel of the RGB565 image and display that in grayscale.

   ``alpha`` controls how opaque the image is. A value of 256 displays an opaque image while a
   value lower than 256 produces a black transparent image. 0 results in a perfectly black image.

   ``color_palette`` if not ``-1`` can be `sensor.PALETTE_RAINBOW`, `sensor.PALETTE_IRONBOW`, or
   a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
   whatever the input image is. This is applied after ``rgb_channel`` extraction if used.

   ``alpha_palette`` if not ``-1`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
   palette which modulates the ``alpha`` value of the input image being displayed at a pixel pixel
   level allowing you to precisely control the alpha value of pixels based on their grayscale value.
   A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
   more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

   ``hint`` can be a logical OR of the flags:

      * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
      * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
      * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
      * `image.CENTER`: Center the image image being displayed on (x, y).
      * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
      * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.

   ``x_size`` may be passed if ``x_scale`` is not passed to specify the size of the image to display
   and ``x_scale`` will automatically be determined passed on the input image size. If neither
   ``y_scale`` or ``y_size`` are specified then ``y_scale`` internally will be set to be equal to
   ``x_size`` to maintain the aspect-ratio.

   ``y_size`` may be passed if ``y_scale`` is not passed to specify the size of the image to display
   and ``y_scale`` will automatically be determined passed on the input image size. If neither
   ``x_scale`` or ``x_size`` are specified then ``x_scale`` internally will be set to be equal to
   ``y_size`` to maintain the aspect-ratio.

   Not supported for compressed images.

.. function:: lcd.clear([display_off=False])

   Clears the lcd screen to black.

   ``display_off`` if True instead turns off the display logic versus clearing the frame LCD
   frame buffer to black. You should also turn off the backlight too after this to ensure the
   screen goes to black as many displays are white when only the backlight is on.

Constants
---------

.. data:: lcd.LCD_NONE

   Returned by `lcd.type()` when the this module is not initialized.

.. data:: lcd.LCD_SHIELD

   Used to initialize the LCD module in SPI LCD drive mode.

.. data:: lcd.LCD_DISPLAY

   Used to initialize the LCD module driving a high resolution display.

.. data:: lcd.LCD_DISPLAY_WITH_HDMI

   Used to initialize the LCD module driving a high resolution display with a secondary mirrored HDMI output.

.. data:: lcd.LCD_DISPLAY_ONLY_HDMI

   Used to initialize the LCD module driving an HDMI output.

.. data:: lcd.QVGA

   320x240 resolution for LCD_DISPLAY* modes.

.. data:: lcd.TQVGA

   240x320 resolution for LCD_DISPLAY* modes.

.. data:: lcd.FHVGA

   480x272 resolution for LCD_DISPLAY* modes.

.. data:: lcd.FHVGA2

   480x128 resolution for LCD_DISPLAY* modes.

.. data:: lcd.VGA

   640x480 resolution for LCD_DISPLAY* modes.

.. data:: lcd.THVGA

   320x480 resolution for LCD_DISPLAY* modes.

.. data:: lcd.FWVGA

   800x480 resolution for LCD_DISPLAY* modes.

.. data:: lcd.FWVGA2

   800x320 resolution for LCD_DISPLAY* modes.

.. data:: lcd.TFWVGA

   480x800 resolution for LCD_DISPLAY* modes.

.. data:: lcd.TFWVGA2

   480x480 resolution for LCD_DISPLAY* modes.

.. data:: lcd.SVGA

   800x600 resolution for LCD_DISPLAY* modes.

.. data:: lcd.WSVGA

   1024x600 resolution for LCD_DISPLAY* modes.

.. data:: lcd.XGA

   1024x768 resolution for LCD_DISPLAY* modes.

.. data:: lcd.SXGA

   1280x1024 resolution for LCD_DISPLAY* modes.

.. data:: lcd.SXGA2

   1280x400 resolution for LCD_DISPLAY* modes.

.. data:: lcd.UXGA

   1600x1200 resolution for LCD_DISPLAY* modes.

.. data:: lcd.HD

   1280x720 resolution for LCD_DISPLAY* modes.

.. data:: lcd.FHD

   1920x1080 resolution for LCD_DISPLAY* modes.

   .. note::

      Use a ``refresh`` of 30 Hz in `lcd.init()` with this setting. The STM32H7 is not capable of
      driving 1080p at 60 Hz.

.. data:: lcd.LCD_GESTURE_MOVE_UP

   Touch screen move up gesture.

.. data:: lcd.LCD_GESTURE_MOVE_LEFT

   Touch screen move left gesture.

.. data:: lcd.LCD_GESTURE_MOVE_DOWN

   Touch screen move down gesture.

.. data:: lcd.LCD_GESTURE_MOVE_RIGHT

   Touch screen move right gesture.

.. data:: lcd.LCD_GESTURE_ZOOM_IN

   Touch screen zoom in gesture.

.. data:: lcd.LCD_GESTURE_ZOOM_OUT

   Touch screen zoom out gesture.

.. data:: lcd.LCD_GESTURE_NONE

   Touch screen no gesture.

.. data:: lcd.LCD_FLAG_PRESSED

   Touch point is pressed.

.. data:: lcd.LCD_FLAG_RELEASED

   Touch point is released.

.. data:: lcd.LCD_FLAG_MOVED

   Touch point is moved.
