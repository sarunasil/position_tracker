import os

class Config(object):
    DEBUG = False
    TESTING = False
    SECRET_KEY = b'$^\x8aF\x92\xf3\xf4\x12\xf9}n\x93L\x17m_m\xbf\xb1\x92\xf6*^\xb1^\x95t\x93\xd0~\t\xf0'

    # SQLALCHEMY_DATABASE_URI = 'sqlite:///instance/prod-db.sqlite'

    # absolute path to this file's root directory
    # FILE_DIR = os.path.dirname(os.path.abspath(__file__))
    # UPLOAD_FOLDER = os.path.join(FILE_DIR, "uploads")

    REMEBER_COOKIE_HTTPONLY = True
    REMEMBER_COOKIE_SECURE = True

class ProdConfig(Config):
    DEFAULT_USER = 'admin'
    # DEFAULT_PASSWORD = None #change this on prod. Will break
    # SECRET_KEY = None #change this on prod. Will break

class DevConfig(Config):
    DEBUG = True

    SECRET_KEY = b"\x13\xe4-\xd6\xba\xb4\x14\x12b4\xde\xd0\xd4\r%QA\xd1\x04\x061\xe6\xaf\x8b\x90s\x1cU'%\xa7\xef"

    # SQLALCHEMY_DATABASE_URI = 'sqlite:///instance/db.sqlite'

    REMEBER_COOKIE_HTTPONLY = False
    REMEMBER_COOKIE_SECURE = False

    # DEFAULT_USER = 'admin'
    # DEFAULT_PASSWORD = 'admin'

class TestConfig(Config):
    TESTING = True