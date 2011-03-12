<?php
/**
 * @file
 * Wrapper for the Emogrifier library.
 * @see http://www.pelagodesign.com/sidecar/emogrifier/
 * Emogrifier converts CSS stylesheets to inline style attributes applied
 * to other HTML tags within the same $message, based on CSS selectors.
 *
 * @param string $message
 *   The message text.  Usually set to $message['body'].
 *
 * @return string $message
 *   The modified message text with inlined CSS applied.
 */
function _htmlmail_emogrify(&$message) {
  if ($emogrifier = &_htmlmail_emogrifier()) {
    $style = array();
    // Pull Drupal-generated linked stylesheets with media="all".
    $link_re = "@<link type=\"text/css\" rel=\"stylesheet\" href=\"(.*)\" media=\"all\" />@Usi";
    if (preg_match_all($link_re, $message, $matches)) {
      foreach ($matches[1] as $url) {
        $response = drupal_http_request($url);
        if (empty($response->error)) {
          $style[] = $response->data;
        }
      }
      $message = str_replace($matches[0], '', $message);
    }
    // Pull out the contents of any style tags.
    $style_re = '@<style[^>]*>\s*?(<!--)??\s*?(.*)\s*?(-->)??\s*?</style>@Usi';
    if (preg_match_all($style_re, $message, $matches)) {
      $style = array_merge($style, $matches[2]);
      // Remove the (now redundant) rules to reduce overall filesize.
      $message = str_replace($matches[0], '', $message);
    }
    // Get rid of comments and whitespace.
    $style = preg_replace(array("/[[:space:]]+/", '#/\*.*?\*/#', '/ +/'), ' ', $style);
    // Emogrify can't handle several CSS rules on one line. As a precaution,
    // we therefore insert LF after each closing bracket.
    $style = preg_replace('/}\s*/', "}\n", implode("\n", $style));
    // Get and reset error levels so we dont get DOMDocument::loadHTML() errors.
    $errorlevel = error_reporting(0);
    // Inline the CSS rules.
    $emogrifier->setHTML($message);
    $emogrifier->setCSS($style);
    $message = $emogrifier->emogrify();
    error_reporting($errorlevel);
  }
  return $message;
}

/**
 * Returns an instance of the Emogrifier class, if available.
 * If needed, loads the appropriate class file.
 *
 * @param &$paths = array()
 *   Additional paths relative to DRUPAL_ROOT to search for the emogrifier.php
 *   library. A default set of search paths will be added.
 *
 * @return
 *   An instance of the Emogrifier class if available, else FALSE.
 */
function &_htmlmail_emogrifier(&$paths = array()) {
  $emogrifier = FALSE;
  if (!is_array($paths)) {
    if (!empty($paths)) {
      $paths = array($paths);
    }
    else {
      $paths = array();
    }
  }
  if (class_exists('Emogrifier')) {
    $emogrifier = new Emogrifier();
  }
  elseif (extension_loaded('dom')) {
    $search = array_flip($paths);
    $search['sites/all/libraries/emogrifier'] = -1;
    $search[drupal_get_path('module', 'htmlmail') . '/emogrifier'] = -2;
    if (function_exists('libraries_get_path')) {
      $search[libraries_get_path('emogrifier')] = -3;
    }
    $paths = array_flip($search);
    ksort($paths);
    foreach ($paths as $dir) {
      if (is_file("$dir/emogrifier.php")) {
        include_once "$dir/emogrifier.php";
        if (class_exists('Emogrifier')) {
          $emogrifier = new Emogrifier();
          break;
        }
      }
    }
  }
  return $emogrifier;
}
