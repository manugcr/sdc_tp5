import time
import matplotlib.pyplot as plt

def read_signal(device):
    with open(device, 'r') as f:
        return int(f.read().strip())

def write_signal(device, pin):
    with open(device, 'w') as f:
        f.write(pin)

def plot_signal(device, pin, duration):
    write_signal(device, pin)
    times = []
    values = []
    start_time = time.time()

    plt.ion()
    fig, ax = plt.subplots()
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('GPIO Value')

    while time.time() - start_time < duration:
        current_time = time.time() - start_time
        signal_value = read_signal(device)
        times.append(current_time)
        values.append(signal_value)

        ax.clear()
        ax.plot(times, values)
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('GPIO Value')
        plt.title(f'GPIO Pin {pin}')
        plt.draw()
        plt.pause(1)

    plt.ioff()
    plt.show()

if __name__ == "__main__":
    device = "/dev/gpio-signal"
    duration = 60  # Duration to plot in seconds
    pin = input("Enter GPIO pin to read (1 or 2): ")
    plot_signal(device, pin, duration)
