import urllib


def compute_config_from_endpoint(endpoint):
    if not endpoint:
        return {}
    res = {'endpoint_url': endpoint}
    url = urllib.parse.urlparse(endpoint)
    if url.scheme == 'http':
        res['use_ssl'] = False
        res['verify'] = False
    return res
