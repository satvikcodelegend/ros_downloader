#include <QApplication>
#include <QMainWindow>
#include <QProcess>
#include <QObject>
#include<QDebug>
#include <stdlib.h>
#include<QLabel>
#include <QVBoxLayout>
#include <stdio.h>
#include <unistd.h>
#include <QPlainTextEdit>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QProgressBar>
#include <QTimer>
#include <QNetworkConfigurationManager>
int main(int argc, char *argv[])
{

	QString script = R"( 
#!/bin/bash
set -e
echo "ROS2 Jazzy Installation"
echo "UBUNTU 24.40 LTS"

echo "[1/8] Updating system"
apt-get update
apt upgrade -y
echo "[2/8] Installing required packages"
apt-get install -y \
  software-properties-common \
  curl \
  gnupg \
  lsb-release
  echo "[3/8] Adding ROS 2 GPG key"
curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
  -o /usr/share/keyrings/ros-archive-keyring.gpg
  echo "[4/8] Adding ROS 2 repository..."
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] \
http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | \
tee /etc/apt/sources.list.d/ros2.list > /dev/null
echo "[4/8] Adding ROS 2 repository..."
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] \
http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | \
tee /etc/apt/sources.list.d/ros2.list > /dev/null

echo "[5/8] Updating package list"
apt update

echo "[6/8] Installing ROS 2 Jazzy Desktop"
apt install -y ros-jazzy-desktop
echo "[7/8] Setting up ROS environment..."
if ! grep -q "source /opt/ros/jazzy/setup.bash" ~/.bashrc; then
  echo "source /opt/ros/jazzy/setup.bash" >> ~/.bashrc
fi
source /opt/ros/jazzy/setup.bash
echo "[8/8] Verifying installation..."
ros2 doctor
echo " ROS 2 Jazzy Installed Successfully"
)";








    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("ROS Downloader");
    
    window.resize(600, 400);
    QPlainTextEdit *outputBox = new QPlainTextEdit();
    outputBox->setReadOnly(true);
    QPushButton *startButton = new QPushButton("Start");
    QPushButton *pauseButton = new QPushButton("Pause");
    pauseButton->setEnabled(false);
    QLineEdit *passwordInput = new QLineEdit();
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setPlaceholderText("Enter sudo password");
    
    QWidget *centralWidget = new QWidget(&window);
	QVBoxLayout* layout = new QVBoxLayout(centralWidget);
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(startButton);
	buttonLayout->addWidget(pauseButton);

	layout->addWidget(passwordInput);
	layout->addLayout(buttonLayout);
    layout->addWidget(outputBox);
    window.setCentralWidget(centralWidget);
    window.show();
    


QProcess *myProcess = new QProcess(&window);
QObject::connect(startButton, &QPushButton::clicked, [=]() {

    if (passwordInput->text().isEmpty()) {
        outputBox->appendPlainText("Password required.");
        return;
    }

    startButton->setEnabled(false);
    pauseButton->setEnabled(true);

    myProcess->start("sudo", QStringList() << "-S" << "bash" << "-c" << script);

});


QObject::connect(myProcess, &QProcess::readyReadStandardOutput, [myProcess,outputBox] {
  outputBox->appendPlainText(
  	myProcess->readAllStandardOutput()
  	);
});
QObject::connect(myProcess, &QProcess::readyReadStandardError, [=]() {

    QString err = myProcess->readAllStandardError();
    outputBox->appendPlainText(err);

    if (err.toLower().contains("password")) {
        myProcess->write((passwordInput->text() + "\n").toUtf8());
    }
});
QObject::connect(pauseButton, &QPushButton::clicked, [=]() {

    if (pauseButton->text() == "Pause") {
        myProcess->kill();   
        pauseButton->setText("Resume");
    } else {
        myProcess->start("sudo", QStringList() << "-S" << "bash" << "-c" << script);
        pauseButton->setText("Pause");
    }

});
QTimer *networkTimer = new QTimer(&window);
networkTimer->start(3000); 
QObject::connect(networkTimer, &QTimer::timeout, [=]() {

    QNetworkConfigurationManager mgr;

    if (!mgr.isOnline()) {
        outputBox->appendPlainText("Internet lost.");
        myProcess->kill();
        pauseButton->setText("Resume");
    }

});


    return app.exec();
}