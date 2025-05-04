#!/bin/bash

finish() {
    echo "Stopping containers..."
    docker-compose down --remove-orphans
    exit 0
}

trap finish SIGINT SIGTERM

CLIENT_COUNT=$1

if [ -z "$CLIENT_COUNT" ]; then
    echo "Usage: ./run.sh <client_count>"
    exit 1
fi

docker-compose build
docker-compose up -d --scale client=$CLIENT_COUNT

sleep 5

SESSION_NAME="multicast_session"

tmux has-session -t $SESSION_NAME 2>/dev/null
if [ $? != 0 ]; then
    tmux new-session -d -s $SESSION_NAME "docker-compose logs -f server"

    tmux split-window -v -t $SESSION_NAME
    tmux send-keys -t $SESSION_NAME "docker-compose logs -f client" C-m
fi

tmux attach -t $SESSION_NAME
