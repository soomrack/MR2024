from PyQt6 import QtWidgets, uic
from PyQt6.QtCore import QTimer
from PyQt6.QtWidgets import QFileDialog
import serial.tools.list_ports
import serial
import sys
import os


class PlotterControl:
    def __init__(self, ui):
        self.ui = ui
        self.serialInst = serial.Serial()

        self.CURRENT_X = 0.0
        self.CURRENT_Y = 0.0
        self.pen_is_down = False

        self.gcode_queue = []
        self.is_running = False
        self.is_paused = False

        self.setup_ui()
        self.setup_serial_monitor()

    def setup_ui(self):
        ports = serial.tools.list_ports.comports()
        self.ui.com_port_choose.clear()
        for port in ports:
            self.ui.com_port_choose.addItem(port.device)

        self.ui.home.clicked.connect(self.home_position)

        self.ui.ButtonUp.clicked.connect(self.move_forward_y)
        self.ui.ButtonDown.clicked.connect(self.move_back_y)
        self.ui.ButtonRight.clicked.connect(self.move_forward_x)
        self.ui.ButtonLeft.clicked.connect(self.move_back_x)

        self.ui.up_right.clicked.connect(self.move_up_right)
        self.ui.up_left.clicked.connect(self.move_up_left)
        self.ui.down_left.clicked.connect(self.move_down_left)
        self.ui.down_right.clicked.connect(self.move_down_right)

        self.ui.zero_x.clicked.connect(self.zero_x)
        self.ui.zero_y.clicked.connect(self.zero_y)

        self.ui.pen_control.clicked.connect(self.toggle_pen)

        self.ui.send_message.clicked.connect(self.send_console_input)
        self.ui.open_port.clicked.connect(self.open_serial)
        self.ui.close_port.clicked.connect(self.close_serial)
        self.ui.refresh_port.clicked.connect(self.refresh_ports)

        self.ui.start_program.clicked.connect(self.start_gcode_execution)
        self.ui.pause_program.clicked.connect(self.pause_gcode_execution)
        self.ui.stop_program.clicked.connect(self.stop_gcode_execution)

        self.ui.actionChoose_file.triggered.connect(self.choose_file)

        self.gcode_timer = QTimer()
        self.gcode_timer.timeout.connect(self.send_next_gcode_line)

        self.timer = QTimer()
        self.timer.timeout.connect(self.read_serial_data)

        self.response_timer = QTimer()
        self.response_timer.timeout.connect(self.check_serial_response)
        self.response_timer.start(1)

    def port_disconnection(self):
        """Обрабатывает отключение порта, закрывает соединение и останавливает таймеры."""
        if self.serialInst.is_open:
            try:
                self.serialInst.close()
            except Exception as e:
                pass
        self.timer.stop()
        self.gcode_timer.stop()
        self.stop_gcode_execution()
        self.ui.consol_monitor.append("Порт отключен")

    def open_serial(self):
        try:
            if self.serialInst.is_open:
                self.serialInst.close()

            port = self.ui.com_port_choose.currentText()
            if not port:
                return

            self.serialInst.port = port
            self.serialInst.baudrate = 115200
            self.serialInst.open()
            self.timer.start(100)
            self.ui.consol_monitor.append(f"Подключено к {port}")

        except Exception as e:
            self.ui.consol_monitor.append(f"Ошибка подключения: {str(e)}")
            self.port_disconnection()

    def close_serial(self):
        try:
            self.port_disconnection()
            self.ui.consol_monitor.append("Порт закрыт")
        except Exception as e:
            self.ui.consol_monitor.append(f"Ошибка: {str(e)}")

    def refresh_ports(self):
        ports = serial.tools.list_ports.comports()
        self.ui.com_port_choose.clear()
        for port in ports:
            self.ui.com_port_choose.addItem(port.device)
        self.ui.consol_monitor.append("Список портов обновлен")

    def setup_serial_monitor(self):
        self.ui.step_size_xy.setRange(1, 1000)
        self.ui.feed_rate.setRange(1, 10000)

    def send_console_input(self):
        text = self.ui.consol_input.toPlainText().strip()
        if text and self.serialInst.is_open:
            self.send_command(text)
            self.ui.consol_input.clear()
        elif not self.serialInst.is_open:
            self.ui.consol_monitor.append("Ошибка: порт не открыт!")

    def read_serial_data(self):
        try:
            if self.serialInst.is_open and self.serialInst.in_waiting:
                data = self.serialInst.readline().decode().strip()
                if data:
                    self.ui.consol_monitor.append(f"{data}")
        except (serial.SerialException, UnicodeDecodeError) as e:
            if isinstance(e, serial.SerialException):
                self.port_disconnection()
            else:
                self.ui.consol_monitor.append("Ошибка декодирования данных")

    def get_step_size(self):
        return self.ui.step_size_xy.value()

    def get_feed_rate(self):
        return self.ui.feed_rate.value()

    def send_command(self, command):
        try:
            if self.serialInst.is_open:
                self.serialInst.write(f"{command}\n".encode())
                self.ui.consol_monitor.append(f"{command}")
        except serial.SerialException as e:
            self.ui.consol_monitor.append(f"Ошибка отправки команды: {str(e)}")
            self.port_disconnection()

    def home_position(self):
        self.send_command("G0X0Y0")
        self.CURRENT_X = 0.0
        self.CURRENT_Y = 0.0
        self.ui.x_current_position.setText("0.00")
        self.ui.y_current_position.setText("0.00")

    def move_forward_x(self):
        step = self.get_step_size()
        feed = self.get_feed_rate()
        self.send_command(f"G21G91G1X{step}F{feed}")
        self.CURRENT_X += step
        self.ui.x_current_position.setText(f"{round(self.CURRENT_X, 2)}")

    def move_back_x(self):
        step = self.get_step_size()
        feed = self.get_feed_rate()
        self.send_command(f"G21G91G1X-{step}F{feed}")
        self.CURRENT_X -= step
        self.ui.x_current_position.setText(f"{round(self.CURRENT_X, 2)}")

    def move_forward_y(self):
        step = self.get_step_size()
        feed = self.get_feed_rate()
        self.send_command(f"G21G91G1Y{step}F{feed}")
        self.CURRENT_Y += step
        self.ui.y_current_position.setText(f"{round(self.CURRENT_Y, 2)}")

    def move_back_y(self):
        step = self.get_step_size()
        feed = self.get_feed_rate()
        self.send_command(f"G21G91G1Y-{step}F{feed}")
        self.CURRENT_Y -= step
        self.ui.y_current_position.setText(f"{round(self.CURRENT_Y, 2)}")

    def move_up_right(self):
        step_x = self.get_step_size()
        step_y = self.get_step_size()
        feed = self.get_feed_rate()
        self.send_command(f"G21G91G1X{step_x}Y{step_y}F{feed}")
        self.CURRENT_X += step_x
        self.CURRENT_Y += step_y
        self.update_position()

    def move_up_left(self):
        step_x = -self.get_step_size()
        step_y = self.get_step_size()
        feed = self.get_feed_rate()
        self.send_command(f"G21G91G1X{step_x}Y{step_y}F{feed}")
        self.CURRENT_X += step_x
        self.CURRENT_Y += step_y
        self.update_position()

    def move_down_left(self):
        step_x = -self.get_step_size()
        step_y = -self.get_step_size()
        feed = self.get_feed_rate()
        self.send_command(f"G21G91G1X{step_x}Y{step_y}F{feed}")
        self.CURRENT_X += step_x
        self.CURRENT_Y += step_y
        self.update_position()

    def move_down_right(self):
        step_x = self.get_step_size()
        step_y = -self.get_step_size()
        feed = self.get_feed_rate()
        self.send_command(f"G21G91G1X{step_x}Y{step_y}F{feed}")
        self.CURRENT_X += step_x
        self.CURRENT_Y += step_y
        self.update_position()

    def update_position(self):
        self.ui.x_current_position.setText(f"{round(self.CURRENT_X, 2)}")
        self.ui.y_current_position.setText(f"{round(self.CURRENT_Y, 2)}")

    def zero_x(self):
        self.CURRENT_X = 0.0
        self.ui.x_current_position.setText("0.00")
        self.send_command("G92 X0")

    def zero_y(self):
        self.CURRENT_Y = 0.0
        self.ui.y_current_position.setText("0.0")
        self.send_command("G92 Y0")

    def toggle_pen(self):
        try:
            if not self.serialInst.is_open:
                self.ui.consol_monitor.append("Ошибка: порт не открыт!")
                return

            if self.pen_is_down:
                self.send_command("M5")
                self.ui.pen_state.setText("UP")
            else:
                self.send_command("M3 S90")
                self.ui.pen_state.setText("DOWN")

            self.pen_is_down = not self.pen_is_down

        except serial.SerialException as e:
            self.port_disconnection()

    def check_serial_response(self):
        try:
            if self.serialInst.is_open and self.serialInst.in_waiting:
                data = self.serialInst.readline().decode().strip()
                if "ok" in data.lower() and self.gcode_queue:
                    self.send_next_gcode_line()
                self.ui.consol_monitor.append(f"{data}")
        except (Exception, serial.SerialException) as e:
            if isinstance(e, serial.SerialException):
                self.port_disconnection()
            else:
                self.ui.consol_monitor.append(f"Ошибка чтения: {str(e)}")

    def start_gcode_execution(self):
        if not self.serialInst.is_open:
            self.ui.consol_monitor.append("Ошибка: порт не открыт!")
            return

        if not self.is_running:
            self.gcode_queue = self.ui.code_from_file.toPlainText().splitlines()
            self.is_running = True
            self.is_paused = False
            self.ui.consol_monitor.append("Старт выполнения G-кода")
            self.send_next_gcode_line()
        elif self.is_paused:
            self.is_paused = False
            self.ui.consol_monitor.append("Продолжение выполнения")
            self.send_next_gcode_line()

    def pause_gcode_execution(self):
        if self.is_running and not self.is_paused:
            self.is_paused = True
            self.ui.consol_monitor.append("Пауза")

    def stop_gcode_execution(self):
        self.gcode_queue.clear()
        self.is_running = False
        self.is_paused = False
        self.ui.consol_monitor.append("Выполнение прервано")

    def send_next_gcode_line(self):
        if self.is_running and not self.is_paused and self.gcode_queue:
            line = self.gcode_queue.pop(0).strip()
            if line:
                self.send_command(line)
            else:
                self.send_next_gcode_line()

    def choose_file(self):
        file_name, _ = QFileDialog.getOpenFileName(None, "Open G-code File", "", "G-code Files (*.gcode *.txt)")
        if file_name:
            try:
                with open(file_name, 'r') as file:
                    gcode = file.read()
                    self.ui.code_from_file.setPlainText(gcode)
            except Exception as e:
                self.ui.consol_monitor.append(f"Ошибка чтения файла: {e}")

def main():
    app = QtWidgets.QApplication([])

    if getattr(sys, 'frozen', False):
        base_dir = sys._MEIPASS
    else:
        base_dir = os.path.dirname(__file__)

    ui_path = os.path.join(base_dir, 'Plotter.ui')
    ui = uic.loadUi(ui_path)

    ui.setWindowTitle("Plotter Controller")
    controller = PlotterControl(ui)
    ui.show()
    app.exec()

if __name__ == "__main__":
    main()
