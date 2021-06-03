# Comandos importantes Docker

## Arrancar mosquitto
### Crear directorios necesarios 
- $(pwd)/mosquitto/conf/mosquitto.conf  
- $(pwd)/mosquitto/log  
- $(pwd)/mosquitto/data  

### Lanzar docker mosquitto 
- *docker run --name mosquitto -p 1883:1883 -v $(pwd)/config/mosquitto.conf:/mosquitto/config/mosquitto.conf -v $(pwd)/log:/mosquitto/log -v $(pwd)/data:/mosquitto/data --network host eclipse-mosquitto* 

### Ejemplo mosquitto pub sub
- *mosquitto_pub -h [ip] -t [topic] -m [msg]*
- *mosquitto_sub -h [ip] -t [topic]*


