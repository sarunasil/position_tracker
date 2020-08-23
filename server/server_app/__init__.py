import os
from os.path import join

from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_login import LoginManager
from flask_wtf.csrf import CSRFProtect

#init SQLAlchemy so we can use it later in our models
db = SQLAlchemy()

csrf = CSRFProtect()

def create_app():
    app = Flask(__name__)

    app.config.from_object("config.DevConfig")

    db.init_app(app)

    login_manager = LoginManager()
    login_manager.login_view = 'auth.login'
    login_manager.session_protection = "strong"
    login_manager.init_app(app)

    csrf.init_app(app)

    from .models import User
    @login_manager.user_loader
    def load_user(user_id):
        return User.query.get(int(user_id))

    #blueprint for auth routes in our app
    # from .auth import auth as auth_blueprint
    # app.register_blueprint(auth_blueprint)

    #blueprint for non-auth parts of app
    from .main import main as main_blueprint
    app.register_blueprint(main_blueprint)

    #blueprint for api routes
    from .api import api as api_blueprint
    csrf.exempt(api_blueprint)
    app.register_blueprint(api_blueprint)

    #blueprint for admin
    # from .admin import adm as admin_blueprint
    # app.register_blueprint(admin_blueprint)

    return app