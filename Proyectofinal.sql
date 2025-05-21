-- Crear base de datos si no existe
IF DB_ID('Chatdatabase') IS NULL
BEGIN
    CREATE DATABASE Chatdatabase
    PRINT 'Base de datos Chatdatabase creada correctamente'
END
GO

-- Usar la base de datos
USE Chatdatabase
GO

-- Crear tabla si no existe
IF OBJECT_ID('Conocimiento', 'U') IS NULL
BEGIN
    CREATE TABLE Conocimiento (
        Id INT PRIMARY KEY IDENTITY(1,1),
        Pregunta NVARCHAR(500) NOT NULL,
        Respuesta NVARCHAR(MAX) NOT NULL,
        FechaCreacion DATETIME DEFAULT GETDATE()
    )
    PRINT 'Tabla Conocimiento creada correctamente'
END
GO

DELETE FROM Conocimiento
WHERE Pregunta IN (
	N'�Qu� tarjeta gr�fica es compatible con un procesador i3-13100F?',
	N'�El i3-13100F soporta memoria DDR5?',
	N'�Necesito refrigeraci�n l�quida para el i3-13100F?',
    N'�Qu� placas madre son compatibles con el i3-13100F?',
    N'�El i3-13100F tiene gr�ficos integrados?',
    N'�Qu� fuente de alimentaci�n necesito para un i3-13100F con RTX 3060?',
    N'�Puedo hacer overclock al i3-13100F?',
    N'�Qu� disipador incluye el i3-13100F?',
    N'�El i3-13100F soporta PCIe 5.0?',
    N'�Qu� velocidad de RAM m�xima soporta el i3-13100F?',
    N'�Es compatible el i3-13100F con Windows 11?',
    N'�Qu� temperatura m�xima soporta el i3-13100F?',
    N'�Cu�ntos n�cleos tiene el i3-13100F?',
    N'�El i3-13100F soporta Thunderbolt?',
    N'�Qu� rendimiento tiene el i3-13100F en juegos?',
    N'�Puedo usar el i3-13100F para streaming?',
    N'�El i3-13100F incluye WiFi?',
    N'�Qu� generaci�n es el i3-13100F?',
    N'�El i3-13100F soporta resoluci�n 4K?',
    N'�Es mejor el i3-13100F que un Ryzen 5 5600G?',
    N'�Qu� motherboard es compatible con un i5-12400F?',
    N'�Qu� tipo de RAM necesita un Ryzen 5 5600X?',
    N'�Es compatible una RTX 3080 con una fuente de 550W?',
    N'�Qu� refrigeraci�n necesito para un i7-13700K?',
    N'�Puedo usar DDR5 en una placa base DDR4?',
    N'�Qu� procesadores son compatibles con socket AM5?',
    N'�Necesito actualizaci�n de BIOS para Ryzen 5000 en B550?',
    N'�Es compatible PCIe 5.0 con PCIe 4.0?',
    N'�Qu� fuente necesito para una RTX 4090?',
    N'�Puedo mezclar RAM de diferentes velocidades?',
    N'�Qu� tama�o de gabinete necesito para una ATX?',
    N'�Es compatible M.2 NVMe con SATA?',
    N'�Puedo usar una GPU de 3 slots en un case de 2 slots?',
    N'�Qu� conectores necesita una motherboard moderna?',
    N'�Puedo usar un cooler de AM4 en AM5?',
    N'�Qu� tipo de almacenamiento es m�s r�pido?',
    N'�Es compatible Thunderbolt con USB-C?',
    N'�Necesito drivers especiales para Windows 11?',
    N'�Puedo conectar un monitor 4K a cualquier GPU?',
    N'�Qu� latencia de RAM es mejor?',
    N'�Puedo usar una fuente SFX en un case ATX?',
    N'�Es compatible una GPU de 300W con mi fuente?',
    N'�Qu� temperatura m�xima soporta un Ryzen 7 5800X?',
    N'�Puedo usar un SSD M.2 en un puerto PCIe?',
    N'�Qu� diferencia hay entre chipset B y X en Intel?',
    N'�Es necesario thermal paste para el CPU?',
    N'�Puedo usar DDR4 de 3600MHz en un i3-12100?',
    N'�Qu� significa TDP en procesadores?',
    N'�Es compatible una GPU vieja con PCIe 3.0?',
    N'�Qu� ventiladores necesito para mi gabinete?',
    N'�Puedo usar un HDD de 2.5 en un desktop?',
    N'�Qu� es el chipset de una motherboard?',
    N'�Es compatible USB 3.2 con USB 2.0?',
    N'�Puedo instalar Windows 10 en un NVMe?',
    N'�Qu� es mejor para gaming DDR4 o DDR5?',
    N'�Puedo conectar 2 monitores a una GPU?',
    N'�Qu� es SLI y CrossFire?',
    N'�Es compatible un monitor 144Hz con HDMI?',
    N'�Puedo usar una laptop RAM en desktop?',
    N'�Qu� es mejor air cooling o liquid cooling?',
    N'�Puedo usar una PSU de 20 a�os?',
    N'�Qu� es PCIe x16?',
    N'�Es compatible un M.2 SATA con NVMe?',
    N'�Puedo overclockear con cualquier motherboard?',
    N'�Qu� es el factor de forma ATX?',
    N'�Puedo usar una GPU sin conectores PCIe?',
    N'�Qu� es mejor para productividad m�s n�cleos o m�s velocidad?',
    N'�Es compatible una GPU AMD con un Intel CPU?',
    N'�Puedo usar una CPU sin GPU integrada?',
    N'�Qu� es el turbo boost en CPUs?',
    N'�Es compatible una RTX 3060 con PCIe 4.0?',
    N'�Puedo usar una CPU de 125W en una placa base de 65W?',
    N'�Qu� es dual channel en RAM?',
    N'�Es compatible una CPU LGA1200 en LGA1700?',
    N'�Puedo usar una GPU de 8GB para 4K?',
    N'�Qu� es mejor SSD SATA o HDD?',
    N'�Es compatible una laptop GPU en desktop?',
    N'�Puedo usar una fuente modular en cualquier case?',
    N'�Qu� es el chipset X670E?',
    N'�Es compatible una pantalla 1080p con una GPU 4K?',
    N'�Puedo usar una CPU de 12 n�cleos para gaming?',
    N'�Qu� es el DirectStorage de Windows?',
    N'�Es compatible una placa base E-ATX en un case ATX?',
    N'�Puedo usar un HDD externo como almacenamiento principal?',
    N'�Qu� es el TPM requerido para Windows 11?',
    N'�Es compatible una GPU de 2.5 slots en una placa mini-ITX?',
    N'�Puedo usar una RAM de 32GB en un i5?',
    N'�Qu� es el chipset B760?',
    N'�Es compatible una GPU con solo DisplayPort en monitor HDMI?',
    N'�Puedo usar una CPU AMD en una placa base Intel?',
    N'�Qu� es el resizable BAR?',
    N'�Es compatible una placa base micro-ATX en un case ATX?',
    N'�Puedo usar una refrigeracion de AIO en cualquier case?',
    N'�Qu� es el overclocking de RAM?',
    N'�Es compatible una GPU con solo HDMI en monitor DisplayPort?',
    N'�Puedo usar una fuente de 1000W para un PC de bajo consumo?',
    N'�Qu� es el chipset Z790?',
    N'�Es compatible una CPU de 14nm en socket moderno?',
    N'�Puedo usar un SSD NVMe como RAM?',
    N'�Qu� es el chipset B650?',
    N'�Es compatible una GPU de 350mm en mi case?',
    N'�Puedo usar una placa base sin puertos USB 3.0?',
    N'�Qu� es el chipset A620?',
    N'�Es compatible una CPU de 105W en placa base de 95W?',
    N'�Puedo usar una GPU con solo VGA en monitor moderno?',
    N'�Qu� es el chipset H770?',
    N'�Es compatible una RAM ECC en placa base consumer?',
    N'Qu� tarjeta gr�fica es compatible con un procesador i3-13100F',
	N'Necesito refrigeraci�n l�quida para el i3-13100F',
	N'Qu� placas madre son compatibles con el i3-13100F',
	N'Qu� fuente de alimentaci�n necesito para un i3-13100F con RTX 3060',
	N'Qu� disipador incluye el i3-13100F',
	N'Qu� velocidad de RAM m�xima soporta el i3-13100F',
	N'Qu� temperatura m�xima soporta el i3-13100F',
	N'Cu�ntos n�cleos tiene el i3-13100F',
	N'Qu� rendimiento tiene el i3-13100F en juegos',
	N'Qu� generaci�n es el i3-13100F'
);

DELETE FROM Conocimiento
WHERE Respuesta IN (
    N'S� soporta DDR4-3200 y DDR5-4800 (depende de la placa madre)',
    N'No la versi�n "F" carece de GPU integrada',
    N'No los procesadores Intel no-K no permiten overclock',
    N'S� tiene 16 l�neas PCIe 5.0 y 4 l�neas PCIe 4.0',
    N'S� es totalmente compatible',
    N'Depende de la placa madre el CPU s� es compatible',
    N'S� con GPU dedicada que soporte NVENC/AMF',
    N'No depende de la placa madre o tarjeta adicional',
    N'S� con tarjeta gr�fica dedicada adecuada',
    N'En rendimiento de CPU s� pero el 5600G tiene mejor GPU integrada',
    N'Cualquier GPU moderna (NVIDIA RTX 30/40 AMD RX 6000/7000) con interfaz PCIe 4.0 x16',
    N'No un disipador de aire de gama media es suficiente',
    N'Cualquier placa con socket LGA1700 (H610 B660 H670 Z690 B760 Z790)',
    N'Recomendada 550W-650W 80+ Bronze/Gold',
    N'Incluye el disipador Intel Laminar RM1',
    N'DDR4-3200 en placas DDR4 DDR5-4800 en placas DDR5',
    N'TJunction de 100�C ideal mantener bajo 80�C',
    N'4 n�cleos P-core (8 hilos con Hyper-Threading)',
    N'Excelente para 1080p con GPU dedicada (60+ FPS)',
    N'13� generaci�n (Raptor Lake)'
);

-- Insertar datos de prueba sin signos
BEGIN TRY
    BEGIN TRANSACTION

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Que tarjeta grafica es compatible con un procesador i3 13100F')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Que tarjeta grafica es compatible con un procesador i3 13100F', N'Cualquier GPU moderna NVIDIA RTX 30 40 AMD RX 6000 7000 con interfaz PCIe 40 x16')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'El i3 13100F soporta memoria DDR5')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'El i3 13100F soporta memoria DDR5', N'Si soporta DDR4 3200 y DDR5 4800 depende de la placa madre')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Necesito refrigeracion liquida para el i3 13100F')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Necesito refrigeracion liquida para el i3 13100F', N'No un disipador de aire de gama media es suficiente')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Que placas madre son compatibles con el i3 13100F')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Que placas madre son compatibles con el i3 13100F', N'Cualquier placa con socket LGA1700 H610 B660 H670 Z690 B760 Z790')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'El i3 13100F tiene graficos integrados')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'El i3 13100F tiene graficos integrados', N'No la version F carece de GPU integrada')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Que fuente de alimentacion necesito para un i3 13100F con RTX 3060')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Que fuente de alimentacion necesito para un i3 13100F con RTX 3060', N'Recomendada 550W 650W 80 Plus Bronze Gold')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Puedo hacer overclock al i3 13100F')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Puedo hacer overclock al i3 13100F', N'No los procesadores Intel no K no permiten overclock')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Que disipador incluye el i3 13100F')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Que disipador incluye el i3 13100F', N'Incluye el disipador Intel Laminar RM1')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'El i3 13100F soporta PCIe 50')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'El i3 13100F soporta PCIe 50', N'Si tiene 16 lineas PCIe 50 y 4 lineas PCIe 40')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Que velocidad de RAM maxima soporta el i3 13100F')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Que velocidad de RAM maxima soporta el i3 13100F', N'DDR4 3200 en placas DDR4 DDR5 4800 en placas DDR5')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Es compatible el i3 13100F con Windows 11')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Es compatible el i3 13100F con Windows 11', N'Si es totalmente compatible')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Que temperatura maxima soporta el i3 13100F')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Que temperatura maxima soporta el i3 13100F', N'TJunction de 100C ideal mantener bajo 80C')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Cuantos nucleos tiene el i3 13100F')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Cuantos nucleos tiene el i3 13100F', N'4 nucleos P core 8 hilos con Hyper Threading')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'El i3 13100F soporta Thunderbolt')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'El i3 13100F soporta Thunderbolt', N'Depende de la placa madre el CPU si es compatible')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Que rendimiento tiene el i3 13100F en juegos')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Que rendimiento tiene el i3 13100F en juegos', N'Excelente para 1080p con GPU dedicada 60 FPS')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Puedo usar el i3 13100F para streaming')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Puedo usar el i3 13100F para streaming', N'Si con GPU dedicada que soporte NVENC AMF')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'El i3 13100F incluye WiFi')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'El i3 13100F incluye WiFi', N'No depende de la placa madre o tarjeta adicional')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Que generacion es el i3 13100F')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Que generacion es el i3 13100F', N'13 generacion Raptor Lake')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'El i3 13100F soporta resolucion 4K')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'El i3 13100F soporta resolucion 4K', N'Si con tarjeta grafica dedicada adecuada')
END

IF NOT EXISTS (SELECT 1 FROM Conocimiento WHERE Pregunta = 'Es mejor el i3 13100F que un Ryzen 5 5600G')
BEGIN
    INSERT INTO Conocimiento (Pregunta, Respuesta) 
    VALUES (N'Es mejor el i3 13100F que un Ryzen 5 5600G', N'En rendimiento de CPU si pero el 5600G tiene mejor GPU integrada')
END

    COMMIT TRANSACTION
    PRINT 'Datos de prueba insertados correctamente'
END TRY
BEGIN CATCH
    ROLLBACK TRANSACTION
    PRINT 'Error al insertar datos ' + ERROR_MESSAGE()
END CATCH
GO

-- Verificar contenido
SELECT * FROM Conocimiento ORDER BY Id
GO