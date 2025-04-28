docker-compose up --build

while true; do
  docker-compose logs --follow
done
