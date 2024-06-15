import serial
import pyautogui
from pynput import mouse

monto = 0
buttons_pressed = {
    mouse.Button.left: False,
    mouse.Button.right: False
}

# Establece la conexión serial
ser = serial.Serial('COM3', 9600)  # Reemplaza 'COM7' por el puerto serie correcto
print("Conexión establecida")

def enviar_monto_recarga(monto):
    ser.write(f"{monto}\n".encode())  # Añade un salto de línea al final del valor

#def on_click(x, y, button, pressed):
 #   if button == mouse.Button.left or button == mouse.Button.right:
        # Actualiza el estado del botón
  #      buttons_pressed[button] = pressed
        # Comprueba si ambos botones están presionados
   #     if buttons_pressed[mouse.Button.left] and buttons_pressed[mouse.Button.right]:
           # pyautogui.press('1')
            
# Configura el listener del mouse
#listener = mouse.Listener(on_click=on_click)
#   listener.start()

while True:
    data = ser.readline().decode().strip()
    print("Arduino dice:", data)

    if data == "Ingrese Saldo:":
        monto_recarga = input("Ingrese el monto de recarga: ")
        enviar_monto_recarga(monto_recarga)
    if data == "Moneda Insertada":
        pyautogui.press('5')
        #pyautogui.press('1')
        