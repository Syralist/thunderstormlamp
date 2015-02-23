# -*- coding: utf-8 -*-

from flask import Flask, jsonify, render_template, request
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
    print jsonify(r = R, g = G, b = B)
    return jsonify(r = R, g = G, b = B)

@app.route('/_set_g')
def setG():
    global R
    global G
    global B
    G = request.args.get('G', 0, type=int)
    return jsonify(r = R, g = G, b = B)

@app.route('/_set_b')
def setB():
    global R
    global G
    global B
    B = request.args.get('B', 0, type=int)
    return jsonify(r = R, g = G, b = B)

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == "__main__":
    app.run(host='0.0.0.0',debug=True)
