
from mqtt2excel import MQTT2Excel
import constants

mqtt2excel = MQTT2Excel(constants.MQTT_SERVER_VM, constants.EXCEL_NAME)

mqtt2excel.setRecordsNumber(3)
mqtt2excel.addTopics("node/1/room")
mqtt2excel.start()