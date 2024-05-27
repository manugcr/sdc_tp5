import tkinter as tk
import os

DEVICE_PATH = "/dev/gpio-led"

def write_device(value):
    try:
        with open(DEVICE_PATH, 'w') as device_file:
            device_file.write(str(value))
            print(f"LED turned {'on' if value else 'off'} successfully.")
    except OSError as e:
        print(f"Failed to write to device file: {e}")

def turn_on_led():
    write_device(1)

def turn_off_led():
    write_device(0)

# Create the main window
root = tk.Tk()
root.title("GPIO LED Control")
root.geometry("300x200")  # Set initial window size

# Define custom font
button_font = ("Arial", 12)

# Create and place the buttons
on_button = tk.Button(root, text="Turn On", command=turn_on_led, bg="green", fg="white", font=button_font, bd=3)
on_button.pack(pady=10, padx=20, fill=tk.BOTH, expand=True)

off_button = tk.Button(root, text="Turn Off", command=turn_off_led, bg="red", fg="white", font=button_font, bd=3)
off_button.pack(pady=10, padx=20, fill=tk.BOTH, expand=True)

# Start the Tkinter event loop
root.mainloop()

