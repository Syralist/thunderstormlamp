# -*- coding: utf-8 -*-

from flask import Flask, jsonify, render_template, request
import serial
import struct
serialport = serial.Serial('/dev/ttyAMA0', baudrate=115200, timeout=3.0)
R = 0
G = 0
B = 0

app = Flask(__name__)

@app.route('/_set_r')
def setR():
    global R
    global G
    global B
    R = request.args.get('R', 0, type=int)
    print "R"
    writeserial()
    return jsonify(r = R, g = G, b = B)

@app.route('/_set_g')
def setG():
    global R
    global G
    global B
    G = request.args.get('G', 0, type=int)
    writeserial()
    return jsonify(r = R, g = G, b = B)

@app.route('/_set_b')
def setB():
    global R
    global G
    global B
    B = request.args.get('B', 0, type=int)
    writeserial()
    return jsonify(r = R, g = G, b = B)

@app.route('/')
def index():
    return render_template('index.html')

def writeserial():
    global R
    global G
    global B
    serialport.write(struct.pack("B",R))
    serialport.write(struct.pack("B",G))
    serialport.write(struct.pack("B",B))
    serialport.write(struct.pack("B",255))

if __name__ == "__main__":
    app.run(host='0.0.0.0',debug=True)
