**Haptic_Bracelet_Final_Code**
- codigoFinalBrazalete.ino :  
  Este archivo contiene el código fuente diseñado en Arduino IDE correspondiente al programa que controla el brazalete háptico.  
  El programa recibe información enviada desde el gripper, específicamente los datos obtenidos por los sensores.
  Con base en esta información, el brazalete activa las diferentes modalidades de estimulación háptica, como vibración, presión o electroestimulación, permitiendo al usuario percibir en tiempo real el estado o interacción del gripper con su entorno.

**PCB_Haptic_Bracelet**
- BOM_PCB_Brazalete_Háptico.csv :  
  Archivo con la lista de materiales (BOM) requeridos para el ensamblaje de la PCB del brazalete háptico. Incluye referencias, valores, encapsulados y proveedores sugeridos para cada componente.
  
- Esquematico_Brazalete_Háptico.PNG :  
  Imagen en formato PNG que muestra el esquemático eléctrico de la PCB, detallando las conexiones entre los componentes electrónicos que forman el sistema de retroalimentación háptica.
  
- Gerber_PCB_Brazalete_Háptico.zip :  
  Archivo comprimido con los archivos Gerber necesarios para la fabricación física de la PCB. Estos archivos son utilizados directamente por los fabricantes para producir la tarjeta de circuito impreso.
  
- Proyecto_EasyEDA_Brazalete_Háptico.zip :  
  Archivo comprimido que contiene el proyecto completo en formato EasyEDA, incluyendo el esquemático y el diseño de la PCB. Este archivo permite realizar modificaciones o revisiones al diseño directamente en la plataforma EasyEDA.

**PCB_Haptic_Bracelet**
- showMAC.ino :  
  Este archivo contiene un código diseñado en Arduino IDE diseñado para obtener y mostrar la dirección MAC del módulo ESP32.
