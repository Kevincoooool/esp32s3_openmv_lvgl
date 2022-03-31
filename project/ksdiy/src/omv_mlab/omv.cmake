if(CONFIG_OPENMV_SUPPORT)
get_filename_component(OPENMV_DIR ${PROJECT_DIR}/../../submodule/openmv ABSOLUTE)
set(OMV_SRCS
  ${OPENMV_DIR}/src/omv/alloc/xalloc.c
  ${PROJECT_DIR}/src/omv_mlab/srcs/fb_alloc.c
  ${OPENMV_DIR}/src/omv/alloc/umm_malloc.c
  ${OPENMV_DIR}/src/omv/alloc/dma_alloc.c
  ${PROJECT_DIR}/src/omv_mlab/srcs/unaligned_memcpy.c
  
  ${OPENMV_DIR}/src/omv/common/array.c
  ${OPENMV_DIR}/src/omv/common/ff_wrapper.c
  ${OPENMV_DIR}/src/omv/common/ini.c
  ${OPENMV_DIR}/src/omv/common/ringbuf.c
  ${OPENMV_DIR}/src/omv/common/trace.c
  ${PROJECT_DIR}/src/omv_mlab/srcs/mutex.c
  
  ${OPENMV_DIR}/src/omv/imlib/agast.c
  ${OPENMV_DIR}/src/omv/imlib/apriltag.c
  ${OPENMV_DIR}/src/omv/imlib/bayer.c                     
  ${OPENMV_DIR}/src/omv/imlib/binary.c                    
  ${OPENMV_DIR}/src/omv/imlib/blob.c                      
  ${OPENMV_DIR}/src/omv/imlib/bmp.c                       
  ${OPENMV_DIR}/src/omv/imlib/clahe.c                     
  ${OPENMV_DIR}/src/omv/imlib/collections.c               
  ${OPENMV_DIR}/src/omv/imlib/dmtx.c                      
  ${OPENMV_DIR}/src/omv/imlib/draw.c                      
  ${OPENMV_DIR}/src/omv/imlib/edge.c                      
  ${OPENMV_DIR}/src/omv/imlib/eye.c                       
  ${OPENMV_DIR}/src/omv/imlib/fast.c                      
  ${OPENMV_DIR}/src/omv/imlib/fft.c                       
  ${OPENMV_DIR}/src/omv/imlib/filter.c                    
  ${PROJECT_DIR}/src/omv_mlab/srcs/fmath.c                     
  ${OPENMV_DIR}/src/omv/imlib/font.c                      
  ${PROJECT_DIR}/src/omv_mlab/srcs/framebuffer.c               
  ${OPENMV_DIR}/src/omv/imlib/fsort.c                     
  ${OPENMV_DIR}/src/omv/imlib/gif.c                       
  ${OPENMV_DIR}/src/omv/imlib/haar.c                      
  ${OPENMV_DIR}/src/omv/imlib/hog.c                       
  ${OPENMV_DIR}/src/omv/imlib/hough.c                     
  ${OPENMV_DIR}/src/omv/imlib/imlib.c                     
  ${OPENMV_DIR}/src/omv/imlib/integral.c                  
  ${OPENMV_DIR}/src/omv/imlib/integral_mw.c               
  ${OPENMV_DIR}/src/omv/imlib/jpeg.c                      
  ${OPENMV_DIR}/src/omv/imlib/kmeans.c                    
  ${OPENMV_DIR}/src/omv/imlib/lab_tab.c                   
  ${OPENMV_DIR}/src/omv/imlib/lbp.c                       
  ${OPENMV_DIR}/src/omv/imlib/line.c                      
  ${OPENMV_DIR}/src/omv/imlib/lsd.c                       
  ${OPENMV_DIR}/src/omv/imlib/mathop.c                    
  ${OPENMV_DIR}/src/omv/imlib/mjpeg.c                     
  ${OPENMV_DIR}/src/omv/imlib/orb.c                       
  ${OPENMV_DIR}/src/omv/imlib/phasecorrelation.c          
  ${OPENMV_DIR}/src/omv/imlib/point.c                     
  ${OPENMV_DIR}/src/omv/imlib/pool.c                      
  ${OPENMV_DIR}/src/omv/imlib/ppm.c                       
  ${OPENMV_DIR}/src/omv/imlib/qrcode.c                    
  ${OPENMV_DIR}/src/omv/imlib/qsort.c                     
  ${OPENMV_DIR}/src/omv/imlib/rainbow_tab.c               
  ${OPENMV_DIR}/src/omv/imlib/rectangle.c                 
  ${OPENMV_DIR}/src/omv/imlib/selective_search.c          
  ${OPENMV_DIR}/src/omv/imlib/sincos_tab.c                
  ${OPENMV_DIR}/src/omv/imlib/stats.c                     
  ${OPENMV_DIR}/src/omv/imlib/template.c                  
  ${OPENMV_DIR}/src/omv/imlib/xyz_tab.c                   
  ${OPENMV_DIR}/src/omv/imlib/zbar.c
  
  ${PROJECT_DIR}/src/omv_mlab/port_esp32s/sensor.c 
  ${PROJECT_DIR}/src/omv_mlab/port_esp32s/arm_dsp_compat.c
)

set(OMV_MOD_SRCS
  ${OPENMV_DIR}/src/omv/modules/py_clock.c
  ${OPENMV_DIR}/src/omv/modules/py_gif.c  
  ${OPENMV_DIR}/src/omv/modules/py_helper.c
  ${PROJECT_DIR}/src/omv_mlab/srcs/py_image.c
  ${OPENMV_DIR}/src/omv/modules/py_imageio.c
  ${OPENMV_DIR}/src/omv/modules/py_mjpeg.c
  ${OPENMV_DIR}/src/omv/modules/py_omv.c  
  ${PROJECT_DIR}/src/omv_mlab/srcs/py_sensor.c
  ${PROJECT_DIR}/src/omv_mlab/srcs/py_lcd.c
)

list(APPEND OMV_SRCS "${OMV_MOD_SRCS}")
if(CONFIG_USB_ENABLED)
list(APPEND OMV_SRCS ${PROJECT_DIR}/src/omv_mlab/srcs/usbdbg.c)
endif()

set(OMV_INCLUDEDIRS
  ${PROJECT_DIR}/src/omv_mlab/inc
  ${PROJECT_DIR}/src/omv_mlab/board_esp32s
  ${OPENMV_DIR}/src/omv/alloc
  ${OPENMV_DIR}/src/omv/common
  ${OPENMV_DIR}/src/omv/imlib
  ${OPENMV_DIR}/src/omv/modules  
  ${MICROPY_DIR}/lib/oofatfs
)
endif()
