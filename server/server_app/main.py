from flask import Blueprint, render_template, url_for, redirect
from flask_login import login_required, current_user

main = Blueprint('main', __name__)

# @main.route('/', defaults={'path': ''}, methods=["POST","GET"])
# @main.route('/<path:path>', methods=["POST", "GET"])
# def default(path):
#     return redirect(url_for('conv.converter'))

@main.route('/')
def index():
    return render_template('index.html')

