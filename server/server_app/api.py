from flask import Blueprint, render_template, url_for, redirect
from flask_login import login_required, current_user

api = Blueprint('api', __name__)

@api.route('/api')
def api_get():
    return render_template('api.html')

@api.route('/api', methods=["POST"])
def api_post():
    pass



