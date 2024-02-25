from functools import wraps
from inspect import signature

import ctypes, os
def run_elevated(func):
    def wrapper(*args, **kwargs):
        try:
            is_admin = os.getuid() == 0
        except AttributeError:
            is_admin = ctypes.windll.shell32.IsUserAnAdmin() != 0
        if is_admin:
            return func(*args, **kwargs)
        else:
            raise "This function must be run as an administrator"
    # preserve signature
    wrapper.__signature__ = signature(func)
    return wrapper

def run_non_elevated(func):
    def wrapper(*args, **kwargs):
        try:
            is_admin = os.getuid() == 0
        except AttributeError:
            is_admin = ctypes.windll.shell32.IsUserAnAdmin() != 0
        if not is_admin:
            return func(*args, **kwargs)
        else:
            raise "This function must not be run as an administrator"
    # preserve signature
    wrapper.__signature__ = signature(func)
    return wrapper