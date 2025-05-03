CLIENT_COUNT=$1

docker-compose down --remove-orphans

docker-compose up -d --scale client=$CLIENT_COUNT

sleep 5

docker-compose logs -f