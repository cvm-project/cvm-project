## How to use

Checkout the main git repository where you want to run the server. Navigate to this folder and create a file `.env` with the following content:

```
GITLAB_PRIVATE_TOKEN=""  # Create this access token in Gitlab, giving it 'read_api' access
GITLAB_TOKEN=""          # Generate this token randomly and provide it to the Gitlab web hook
debug="true"             # include this line to have more verbose messages
```

Then run the following:

```bash
docker-compose up
```
