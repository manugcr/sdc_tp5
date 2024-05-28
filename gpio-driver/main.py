import time
import threading
import queue
import matplotlib.pyplot as plt
import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
 
class GPIOPlotterApp:
    def __init__(self, root, device, default_pin="1"):
        """
        Initialize the GPIOPlotterApp.
 
        Args:
            root (tk.Tk): The root Tkinter window.
            device (str): The device file to read GPIO signals from.
            default_pin (str): The default GPIO pin to start reading from.
        """
        self.root = root
        self.device = device
        self.input_queue = queue.Queue()
        self.current_pin = default_pin
        self.running = True
 
        self.root.title("GPIO Plotter")
        self.create_widgets()
 
        # Set the default GPIO pin
        self.write_signal(self.current_pin)
 
        # Start the plotting thread
        self.plot_thread = threading.Thread(target=self.plot_signal)
        self.plot_thread.start()
 
    def create_widgets(self):
        """
        Create and arrange the widgets in the Tkinter window.
        """
        self.plot_frame = tk.Frame(self.root)
        self.plot_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True)
 
        self.button_frame = tk.Frame(self.root)
        self.button_frame.pack(side=tk.BOTTOM, fill=tk.X)
 
        self.pin1_button = tk.Button(self.button_frame, text="Pin 1", command=lambda: self.change_pin("1"))
        self.pin1_button.pack(side=tk.LEFT)
 
        self.pin2_button = tk.Button(self.button_frame, text="Pin 2", command=lambda: self.change_pin("2"))
        self.pin2_button.pack(side=tk.LEFT)
 
        self.quit_button = tk.Button(self.button_frame, text="Quit", command=self.quit)
        self.quit_button.pack(side=tk.RIGHT)
 
        self.fig, self.ax = plt.subplots()
        self.ax.set_xlabel('Time (s)')
        self.ax.set_ylabel('GPIO Value')
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.plot_frame)
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)
 
    def change_pin(self, pin):
        """
        Change the GPIO pin being read.
 
        Args:
            pin (str): The GPIO pin to read from.
        """
        self.input_queue.put(pin)
 
    def quit(self):
        """
        Stop the plotting and close the application.
        """
        self.running = False
        self.input_queue.put("q")
        self.plot_thread.join()
        self.root.quit()
 
    def read_signal(self):
        """
        Read the signal value from the device.
 
        Returns:
            int: The signal value read from the device.
        """
        with open(self.device, 'r') as f:
            return int(f.read().strip())
 
    def write_signal(self, pin):
        """
        Write the GPIO pin to the device.
 
        Args:
            pin (str): The GPIO pin to write to the device.
        """
        with open(self.device, 'w') as f:
            f.write(pin)
 
    def plot_signal(self):
        """
        Continuously read and plot the signal from the GPIO pin.
        """
        times = []
        values = []
        start_time = time.time()
 
        while self.running:
            if not self.input_queue.empty():
                pin = self.input_queue.get()
                if pin == "q":
                    break
                if pin not in ["1", "2"]:
                    print("Invalid input. Please enter 1 or 2.")
                    continue
 
                if self.current_pin != pin:
                    self.current_pin = pin
                    self.write_signal(pin)
                    times = []
                    values = []
                    start_time = time.time()
                    self.ax.clear()
                    self.ax.set_xlabel("Time (s)")
                    self.ax.set_ylabel("GPIO Value")
                    self.ax.set_title(f"GPIO Pin {pin}")
 
            current_time = time.time() - start_time
            signal_value = self.read_signal()
            times.append(current_time)
            values.append(signal_value)
 
            self.ax.clear()
            self.ax.plot(times, values)
            self.ax.set_xlabel('Time (s)')
            self.ax.set_ylabel('GPIO Value')
            self.ax.set_title(f'GPIO Pin {self.current_pin}')
            self.canvas.draw()
            time.sleep(1)
 
if __name__ == "__main__":
    root = tk.Tk()
    device = "/dev/gpio-signal"
    default_pin = "1"  # Set your default pin here
    app = GPIOPlotterApp(root, device, default_pin)
    root.mainloop()
