DEFAULT_CLIENTS=5

CLIENT_COUNT=${1:-$DEFAULT_CLIENTS}

if ! [[ "$CLIENT_COUNT" =~ ^[0-9]+$ ]]; then
    echo "Error: Argument must be a number"
    echo "Usage: $0 [number_of_clients]"
    echo "Default: $DEFAULT_CLIENTS clients"
    exit 1
fi

chmod +x run.sh

./run.sh "$CLIENT_COUNT"