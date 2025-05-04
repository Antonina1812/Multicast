finish() 
{
    echo "Stopping containers..."
    docker-compose down --remove-orphans
    exit 0
}

trap finish SIGINT SIGTERM

CLIENT_COUNT=$1

docker-compose build
docker-compose up -d --scale client=$CLIENT_COUNT

sleep 5

docker-compose logs -f