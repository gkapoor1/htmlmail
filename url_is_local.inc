<?php

/**
 * Find the appropriate configuration directory for a given host and path.
 *
 * @param $http_host
 *   The hostname and optional port number, e.g. "www.example.com" or
 *   "www.example.com:8080".
 *
 * @param $script_name
 *   The part of the url following the hostname, including the leading slash.
 *
 * @return
 *   The path of the matching directory.
 *
 * @see conf_path()
 */
function _conf_path($http_host, $script_name, $require_settings = TRUE) {
  $confdir = 'sites';

  $sites = array();
  if (file_exists(DRUPAL_ROOT . '/' . $confdir . '/sites.php')) {
    // This will overwrite $sites with the desired mappings.
    include(DRUPAL_ROOT . '/' . $confdir . '/sites.php');
  }

  $uri = explode('/', $script_name);
  $server = explode('.', implode('.', array_reverse(explode(':', rtrim($http_host, '.')))));
  for ($i = count($uri) - 1; $i > 0; $i--) {
    for ($j = count($server); $j > 0; $j--) {
      $dir = implode('.', array_slice($server, -$j)) . implode('.', array_slice($uri, 0, $i));
      if (isset($sites[$dir]) && file_exists(DRUPAL_ROOT . '/' . $confdir . '/' . $sites[$dir])) {
        $dir = $sites[$dir];
      }
      if (file_exists(DRUPAL_ROOT . '/' . $confdir . '/' . $dir . '/settings.php') || (!$require_settings && file_exists(DRUPAL_ROOT . '/' . $confdir . '/' . $dir))) {
        $conf = "$confdir/$dir";
        return $conf;
      }
    }
  }
  $conf = "$confdir/default";
  return $conf;
}

/**
 * Determines whether a given URL could be local to the current Drupal site.
 *
 * Finds the configuration directory matching the given URL, and compares it
 * to the current configuration directory.  Returns a relative URL path if they
 * match, and FALSE if they don't.
 *
 * In a properly-configured multisite installation, this function helps answer
 * the question, "Could this URL match a file or path within my site?"
 *
 * Whether the URL does in fact resolve to this site, or at all, cannot be
 * determined within Drupal itself.  This is only a sanity check.
 *
 * For example, when looking for image URLs within an HTML mail body for
 * possible conversion to inline attachments, the following code might be used:
 * @code
 *   if ( !url_is_external($url)
 *     && ($path = url_is_local($url))
 *     && file_exists($path) ) {
 *     // Attach $path and replace $url.
 *     ...
 *   }
 * @endcode
 *
 * @param $path
 *   The internal path or external URL being linked to, such as "node/34" or
 *   "http://example.com/foo".
 * @return
 *   A relative path, or FALSE.
 *
 * @see conf_path()
 */
function url_is_local($path) {
  $local = &drupal_static(__FUNCTION__, '');

  $url = drupal_parse_url($path);
  $http_host = $url[PHP_URL_HOST];
  if (isset($url[PHP_URL_PORT])) {
    $http_host .= ':' . $url[PHP_URL_PORT];
  }
  $script_name = $url[PHP_URL_PATH];
  $path = $http_host . $script_name;

  if (!isset($local[$path])) {
    $local[$path] = (conf_path() == _conf_path($http_host, $script_name));
  }
  return $local[$path] ? $script_name : FALSE;
}
