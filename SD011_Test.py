import sds011
import time

sensor = sds011.SDS011("COM18", use_query_mode=True)


sensor.sleep( sleep=False )
time.sleep( 15 )
pm25, pm10 = sensor.query()
sensor.sleep()
print( "PM2.5: %6.1f" % pm25 )
print( "PM10: %6.1f" % pm10 )