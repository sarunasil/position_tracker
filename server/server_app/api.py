import json

from flask import Blueprint, render_template, url_for, redirect, request, jsonify
from flask_login import login_required, current_user

api = Blueprint('api', __name__)

@api.route('/api/<d>')
def api_get_data(d):
    data = d

    try:
        with open('post_file.txt', 'a+') as dfile:
            dfile.write(data)
            dfile.write('\n')
            data = "Ok. 0"
    except:
        data = "Err. 5"

    return jsonify(data)

@api.route('/api')
def api_get():
    # return render_template('api.html')

    if request.args.get('data'):
        data = json.dumps(request.args.get('data'))

        try:
            with open('post_file.txt', 'a') as dfile:
                dfile.write(data)
                dfile.write('\n')
            data = "OK. 0"
        except:
            data = "Err. 7"
    else:
        data = ""
        try:
            with open("post_file.txt", 'r') as dfile:
                data = dfile.readlines()
        except:
            data = "Err. 8"

    return jsonify(data)

@api.route('/api', methods=["POST"])
def api_post():
    data = json.dumps(request.get_json())

    try:
        with open('post_file.txt', 'a') as dfile:
            dfile.write(data)
            dfile.write('\n')
        data = "OK. 0"
    except:
        data = "Err. 6"

    return jsonify(data)


