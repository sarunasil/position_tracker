#!/bin/bash

echo "Making sure we're on the release branch"
git checkout release
echo "---"

echo "Pulling newest changes"
git pull
echo "---"

echo "Activating the virtual env in './venv/bin/activate'"
source ./venv/bin/activate
echo "---"

echo "Making sure all packages are just like in 'requirements.txt'"
pip install -r requirements.txt
echo "---"

echo "Running waitress via nohup in background"
nohup waitress-serve --port=51000 server:app &
echo "---"