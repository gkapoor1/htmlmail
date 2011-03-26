<?php
/**
 * @file
 * Implements MailSystemInterface.
 *
 * Declares the HTMLMailSystem class, which formats and sends mail using the
 * Pear Mail_mimePart and Mail_mimeDecode packages.
 *
 * @see http://drupal.org/node/900794
 * @see http://api.drupal.org/api/drupal/includes--mail.inc/interface/MailSystemInterface/7
 * @see http://pear.php.net/manual/en/package.mail.mail-mimepart.php
 * @see http://pear.php.net/manual/en/package.mail.mail-mimedecode.php
 */

// @todo Use libraries API to locate this.
require_once 'Mail/mimePart.php';
require_once 'Mail/mimeDecode.php';

/**
 * Extends Mail_mimePart to add text() and attach() functions.
 */
class HTMLMail_mimePart extends Mail_mimePart {

  var $cids;

  function is_text() {
    return !strncasecmp($this->content_type(), 'text/', 5);
  }

  /**
   * Some debugging functions.
   *
  function dump_mime($prefix='') {
    $output = $prefix . ' ' . str_replace("\n",'',$this->_headers['Content-Type']) . ' '
      . ( $this->is_text() ? $this->_var_more($this->_body) : $this->_encoding )
      . "\n";
    for ($i = 0; $i < count($this->_subparts); $i++) {
      $output .= $this->_subparts[$i]->dump_mime($prefix . '.' . $i);
    }
    return $output;
  }

  function _var_more($text,$limit=20) {
    return var_export(
      preg_replace(
        array(
          '/^(.{' . $limit. ',' . $limit. '}).+$/s',
          '/\n/',
        ),
        array(
          '\1...',
          '\n',
        ),
        $text
      ),1
    );
  }
  * End debugging functions.
  */

  function __construct($body = '', $params = array(), &$parent = NULL) {
    if (isset($parent)) {
      $this->cids = &$parent->cids;
    }
    else {
      $this->cids = array();
    }
    if (isset($params['body_file'])) {
      $params['cid'] = md5($params['body_file']);
      $this->cids[$params['body_file']] = $params['cid'];
    }
    parent::__construct($body, $params);
  }

  function &addSubPart($body, $params) {
    $this->_subparts[] = new HTMLMail_mimePart($body, $params, $this);
    return $this->_subparts[count($this->_subparts)-1];
  }

  function &addMultiPart($subtype) {
    $this->_subparts[] = new HTMLMail_mimePart(
      '', array('content_type' => 'multipart/' . $subtype), $this);
    return $this->_subparts[count($this->_subparts)-1];
  }

  public function copy(HTMLMail_mimePart $source) {
    $this->_encoding = $source->_encoding;
    $this->_subparts = $source->_subparts;
    $this->_encoded = $source->_encoded;
    $this->_headers = $source->_headers;
    $this->_body = $source->_body;
    $this->_eol = $source->_eol;
  }

  /**
   * Finds and/or sets and returns an inline text mime part.
   *
   * Finds and returns the first inline mime (sub-)part whose content sub-type
   * matches the $subtype parameter.  Optionally creates, deletes, or modifies
   * the mime part.
   *
   * If the $text parameter is NULL, the matching mime part is returned.  If no
   * match was found, NULL is returned.
   *
   * If the $text parameter is set and empty, the matching mime part is deleted
   * if found, and FALSE is returned.
   *
   * If the $text parameter is set and non-empty, it is copied to the body field
   * of the matching mime part, which is created if necessary.  The matching mime
   * part is returned.
   *
   * @param $text: The text to be copied into the matching mime part.
   * @param $params: An associative array containing the following keys:
   * - charset: Should be 'UTF-8'.
   * - content_type: The content type, which should begin with 'text/'.
   * - disposition: Should be 'inline'.
   * - eol: Should be variable_get('mail_line_endings', MAIL_LINE_ENDINGS));
   *
   * @return
   *   FALSE if a matching part was found and deleted.
   *   HTMLMail_mimePart if found and not deleted.
   *   NULL if no matching part was found.
   */
  public function &text($text, array $params, $main_part = TRUE) {
    if (($this_type = $this->content_type()) == $params['content_type']) {
      $this->headers['Content-Disposition'] = 'inline';
      if (!empty($text)) {
        $this->_body = $text;
      }
      return $this;
    }
    // This is a depth-first search.
    for ($key = count($this->_subparts); $key > 0;) {
      $key--;
      $return = &$this->_subparts[$key]->text($text, $params, FALSE);
      if ($return instanceof HTMLMail_mimePart) {
        if (is_null($text) || $text) {
          return $return;
        }
        unset($this->_subparts[$key]);
        $this->_subparts = array_values($this->_subparts);
        $null = NULL;
        return $null;
      }
    }
    // No matching part was found.
    if (!empty($text)) {
      // Create a new part.
      if ($this_type == 'multipart/alternative') {
        // Multipart/alternative may hold text.
        return $this->addSubPart($text, $params);
      }
      if ($this_type == 'multipart/related') {
        // Multipart/related may hold multipart/alternative.
        return $this->addMultiPart('alternative')
          ->addSubPart($text, $params);
      }
      if ($this_type == 'multipart/mixed') {
        // Multipart/mixed may hold multipart/related.
        return $this->addMultiPart('related')
          ->addMultiPart('alternative')
          ->addSubPart($text, $params);
      }
      if ($main_part) {
        // Convert $this to multipart/mixed.
        $copy = clone $this;
        $mixed = &$this->addMultiPart('mixed');
        $related = &$mixed->addMultiPart('related');
        $alternative = &$related->addMultiPart('alternative');
        if (strncmp($this_type, 'text/', 5)) {
          // Add non-text parts to multipart/related.
          $related->_subparts[] = &$copy;
        }
        else {
          // Add text parts to multipart/alternative.
          $alternative->_subparts[] = &$copy;
        }
        $this->copy($mixed);
        return $alternative->addSubPart($text, $params);
      }
    }
    $null = NULL;
    return $null;
  }

  /**
   * Finds or adds and returns an attached mime part.
   *
   * Finds and returns the first attached mime (sub-)part whose content-id
   * matches the $params['cid'] value.  Creates the part if it does not already
   * exist.
   *
   * @param $prams: An associative array containing the following keys:
   * - body_file: Filesystem path of file to be attached.
   * - charset: Must be 'UTF-8'.
   * - cid: The content-id, which is generated by md5(body_file).
   * - content_type: The content-type, such as 'image/gif'.
   * - description: Discriptive text to be displayed in attachment views.
   * - disposition: Should be 'attachment'.
   * - encoding: Should be 'base64' for binary attachments.
   * - eol: Should be variable_get('mail_line_endings', MAIL_LINE_ENDINGS).
   * - filename: Short name for file; defaults to basename(body_file).
   * @param $type: The mime content-type of the attachment.  If null and
   * mime-detect is available, will attempt to auto-detect.  Otherwise,
   * will default to 'application/octet-stream'.
   * @param $name: The name to be shown.  Defaults to basename($path).
   * @param $text: The description to be shown.  Defaults to ''.
   *
   * @return HTMLMail_mimePart, or NULL on error.
   */
  public function &attach(array &$params, $sub_part = FALSE) {
    if (($this_type = $this->content_type()) == 'multipart/related') {
      // Prefer to place non-text parts in multipart/related so they are
      // available to HTML using "cid:" urls.
      return $this->addSubPart('', $params);
    }
    for ($key = 0; $key < count($this->_subparts); $key++) {
      if ($return =&$this->_subparts[$key]->attach($params, TRUE)) {
        return $return;
      }
    }
    // No multipart/related parts found.
    if ($this_type == 'multipart/mixed') {
      // Multipart/mixed may hold multipart/related.
      return $this->addMultiPart('related')
        ->addSubPart($params);
    }
    if ($sub_part) {
      $null = NULL;
      return $null;
    }
    // Convert to multipart/mixed.
    $copy = clone $this;
    $mixed = &$this->addMultiPart('mixed');
    $related =&$mixed->addMultiPart('related');
    if (strncmp($this_type, 'text/', 5)) {
      // Add non-text parts to multipart/related.
      $related->_subparts[] = &$copy;
    }
    else {
      // Add text parts to multipart/alternative.
      $alternative = $related->addMultiPart('alternative');
      $alternative->_subparts[] = &$copy;
    }
    $this->copy($mixed);
    return $related->addSubPart('', $params);
  }

  public function content_type($type = NULL) {
    return isset($type)
      ? strtolower(array_shift(explode(';', $type)))
      : isset($this->_headers['Content-Type'])
      ? strtolower(array_shift(explode(';', $this->_headers['Content-Type'])))
      : 'text/plain';
  }

  public function cid() {
    return isset($this->_headers['Content-ID'])
      ? substr($this->_headers['Content-ID'], 1, -1)
      : '';
  }
}

/**
 * Implements MailSystemInterface.
 *
 * Also supplies helper functions for manipulating email messages.
 */
class HTMLMailSystem implements MailSystemInterface {
  /**
   * Returns current mail_line_endings setting.
   */
  public static function eol() {
    static $eol;
    return isset($eol) ? $eol
      : ($eol = variable_get('mail_line_endings', MAIL_LINE_ENDINGS));
  }

  /**
   * Returns an array with keys changed to match the case of email headers.
   */
  public static function str_to_headers($str) {
    $headers = array();
    if (!is_array($str)) {
      $str = mail_mimeDecode::decode(
        array(
          'decode_headers' => TRUE,
          'input' => $str,
        )
      )->headers;
    }
    foreach ($str as $name => $value) {
      $name = preg_replace(
        array(
          '/([[:alpha:]])([[:alpha:]]+)/e',
          '/^Mime-/',
          '/-Id$/',
        ),
        array(
          'strtoupper("\1") . strtolower("\2")',
          'MIME-',
          '-ID',
        ),
        $name
      );
      $headers[$name] = $value;
    }
    return $headers;
  }

  public static function headers_to_str(array $headers) {
    $str = '';
    foreach ($headers as $name => $value) {
      // Hardcoded newline because many mailsystems require it.
      $str .= $name . ': ' . HTMLMail_mimePart::encodeHeader($name, $value, 'UTF-8') . "\n";
    }
    return $str;
  }

  public static function headers_merge($headers, $xtra) {
    return array_merge(
      str_to_headers($headers),
      str_to_headers($xtra)
    );
  }

  public static function body_to_str($body) {
    $body = preg_replace('/(\r|\r\n|\n)/', HTMLMailSystem::eol(), $body);
    if (is_array($body)) {
      $body = implode(HTMLMailSystem::eol(), $body);
    }
    return $body;
  }

  public static function str_to_body($str) {
    return explode(HTMLMailSystem::eol(), $str);
  }

  public function &email_to_mime(array $headers, $body) {
    $email = $this->headers_to_str($headers) . HTMLMailSystem::eol() . $body;
    $mime = &$this->_mimeDecode_to_mimePart(
      Mail_mimeDecode::decode(
        array(
          'decode_headers' => TRUE,
          'decode_bodies' => TRUE,
          'include_bodies' => TRUE,
          'rfc822_bodies' => TRUE,
          'input' => $email,
        )
      )
    );
    if (empty($mime) || PEAR::IsError($mime)) {
      watchdog(
        'HTMLMailSystem',
        'Could not parse email: !email',
        array('!message' => "<pre>\n" . $email),
        WATCHDOG_ERROR
      );
      $null = NULL;
      return $null;
    }
    return $mime;
  }

  public function mime_to_email(array &$message) {
    if ($this->_invalid($message)) {
      return array(array(), '');
    }
    $encoded  = $message['HTMLMail_mimePart']->encode();
    return array(
      array_diff_key($message['headers'], $encoded['headers']) + $encoded['headers'],
      $encoded['body']
    );
  }

  /**
   * Finds and/or sets and returns an inline text mime part.
   *
   * Finds and returns the first inline mime part whose content-type matches the
   * $type parameter.  Optionally creates, deletes, or modifies the mime part.
   *
   * If the $text parameter is NULL, the matching mime part is returned.  If no
   * match was found, NULL is returned.
   *
   * If the $text parameter is set and empty, the matching mime part is deleted
   * if found, and NULL is returned.
   *
   * If the $text parameter is set and non-empty, it is copied to the body field
   * of the matching mime part, which is created if necessary.  The matching mime
   * part is returned.
   *
   * @param $message: The email message array.
   * @param $subtype: The text sub-type to search for.
   * @param $text: The text to be copied into the matching mime part.
   *
   * @return The matching mime part body, or NULL if not found or deleted.
   */
  public function &text(array &$message, $text, $subtype) {
    if ($this->_invalid($message)) {
      $null = NULL;
      return $null;
    }
    $params = array(
      'content_type' => 'text/' . strtolower($subtype),
      'disposition' => 'inline',
      'charset' => 'UTF-8',
      'encoding' => 'quoted-printable',
      'eol' => $this->eol(),
    );
    $subpart = &$message['HTMLMail_mimePart']->text($text, $params);
    return $subpart->_body;
  }

  /**
   * Finds and/or sets and returns an attachment mime part.
   */
  public function attach(array &$message, $path, $type=NULL, $name=NULL, $description=NULL) {
    if ($this->_invalid($message)) {
      $null = NULL;
      return $null;
    }
    if (isset($message['HTMLMail_mimePart']->cids[$path])) {
      return $message['HTMLMail_mimePart']->cids[$path];
    }
    // @todo Auto-discover $type using mime_detect.
    $params = array(
      'body_file' => $path,
      'charset' => 'UTF-8',
      'cid' => md5($path),
      'content_type' => empty($type) ? 'application/octet-stream' : $type,
      'description' => empty($description) ? $path : $description,
      'disposition' => 'attachment',
      'encoding' => 'base64',
      'eol' => $this->eol(),
      'filename' => empty($name) ? basename($path) : $name,
    );
    return ($part = $message['HTMLMail_mimePart']->attach($params))
      ? $part->cid() : NULL;
  }

  /**
   * Format emails according to module settings.
   *
   * Parses body into an HTMLMail_mimePart object.  If another module subsequently
   * modifies the body, then format() should be called again before sending.  This
   * is safe because the $message['body'] is unmodified by this function.
   *
   */
  public function format(array $message) {
    $body = $this->body_to_str($message['body']);
    $headers = $this->str_to_headers($message['headers']);
    // Parse, then rebuild the message.
    if (!($message['HTMLMail_mimePart'] = &$this->email_to_mime($headers, $body))) {
      // Logged unparseable by email_to_mime. What else to do?
      $message = array('headers' => array(), 'body' => '') + $message;
      return $message;
    }
    if ( !($body = $this->text($message, NULL, 'html'))
      && !($body = $this->text($message, NULL, 'plain')) ) {
      $body = '';
    }
    $headers = $message['HTMLMail_mimePart']->_headers;
    if ( (isset($message['format']) && ($filter = $message['format']))
      || ($filter = variable_get('htmlmail_prefilter')) ) {
      $body =  check_markup($body, $filter);
    }
    // Apply the template file.
    $email = $message;
    $email['headers'] = array_diff_key($email['headers'], $headers) + $headers;
    $email['body'] = $body;
    $body = theme(
      'htmlmail',
      $email
    );
    // Store the plaintext version.
    $this->text($message, drupal_html_to_text($body), 'plain');
    // Check to see whether recipient allows non-plaintext.
    if ( !($recipient = user_load_by_mail($message['to']))
      || empty($recipient->data['htmlmail_plaintext']) ) {
      // Optionally apply the selected theme.
      if ($theme = htmlmail_get_selected_theme($message)) {
        // Strip javascript as it doesn't work through email anyway.
        $body = preg_replace(
          '@<script type="text/javascript".*</script>@Usi',
          '',
          echo_themed_page($message['subject'], $body, $theme)
        );
      }
      // Optionally apply the selected output filter.
      if ($filter = variable_get('htmlmail_postfilter')) {
        $body = check_markup($body, $filter);
      }
      // Attach page requisites and replace absolute urls with cid: urls.
      $this->attach_requisites($message, $body);
      // Clear, then store html version.
      $this->text($message, '', 'html');
      $this->text($message, $body, 'html');
    }
    return $message;
  }

  /**
   * Send an email message.
   */
  public function mail(array $message) {
    // If message was not encoded, abort.
    if ($this->_invalid($message)) {
      return FALSE;
    }
    // Decode from mimePart back into headers and body.
    list($headers, $body) = $this->mime_to_email($message);
    // Ensure that subject is non-null.
    $message += array('subject' => t('(No subject)'));
    // Check for empty recipient.
    if (empty($message['to'])) {
      if (empty($message['headers']['To'])) {
        watchdog(
          'HTMLMailSystem',
          'Cannot send email about %subject without a recipient.',
          array('subject' => $message['subject']),
          WATCHDOG_ERROR
        );
        return FALSE;
      }
      $message['to'] = $message['headers']['To'];
    }
    // Check for empty body.
    if (empty($body)) {
      watchdog(
        'HTMLMailSystem',
        'Refusing to send a blank email to %recipient about %subject.',
        array('%recipient' => $message['to'], '%subject' => $message['subject']),
        WATCHDOG_WARNING
      );
      return FALSE;
    }
    $extra = (
      isset($headers['Return-Path'])
      && !ini_get('safe_mode')
      && !strpos(ini_get('sendmail_path'), ' -f')
    ) ? '-f ' . $headers['Return-Path']
      : '';
    return mail(
      HTMLMail_mimePart::encodeHeader('to', $message['to'], 'UTF-8'),
      HTMLMail_mimePart::encodeHeader('subject', $message['subject'], 'UTF-8'),
      $body,
      $this->headers_to_str($headers),
      $extra
    );
  }

  /**
   * Returns TRUE if a message could not be formatted.
   */
  private function _invalid(array &$message) {
    return (!isset($message['HTMLMail_mimePart']));
  }

  /**
   * Creates a HTMLMail_mimePart object from a Mail_mimeDecode::decode object.
   */
  private function &_mimeDecode_to_mimePart(stdClass $decoded, &$mimePart = NULL) {
    $body = empty($decoded->body)
      ? '' : $decoded->body;
    $params = array(
      'content_type' => $decoded->headers['content-type'],
      'eol' => HTMLMailSystem::eol(),
    );
    if (!empty($decoded->headers['content-transfer-encoding'])) {
      $params['encoding'] = $decoded->headers['content-transfer-encoding'];
    }
    if (!empty($decoded->ctype_parameters['charset'])) {
      $params['charset'] = $decoded->ctype_parameters['charset'];
    }
    if (!empty($decoded->headers['content-id'])) {
      $params['cid'] = $decoded->headers['content-id'];
    }
    if (!empty($decoded->disposition)) {
      $params['disposition'] = $decoded->disposition;
    }
    if (!empty($decoded->d_parameters['filename'])) {
      $params['dfilename'] = $decoded->d_parameters['filename'];
    }
    if (!empty($decoded->d_parameters['name'])) {
      $params['description'] = $decoded->d_parameters['name'];
    }
    if (is_null($mimePart)) {
      $mimePart = new HTMLMail_mimePart($body, $params);
    }
    else {
      $mimePart = &$mimePart->addSubPart($body, $params);
    }
    if (!empty($decoded->parts)) {
      foreach ($decoded->parts as $part) {
        $this->_mimeDecode_to_mimePart($part, $mimePart);
      }
    }
    return $mimePart;
  }

  public function attach_requisites(array &$message, &$body) {
    if ($this->_invalid($message)) {
      return FALSE;
    }
    $this->_message =&$message;
    $search = array(
      '#(?<!\S)(src|background|href)\s*(=)\s*(["\'])(?!cid:)(.*?)(\3)#i',
      '#(?<!\S)(src|background|href)\s*(=)(\s*)(?!"cid:)([^ >]*)(?=[ >])#i',
      '#(?<!\S)(url)\s*(\()\s*(["\'])(?!cid:)(.*?)(\3)#i',
      '#(?<!\S)(url)\s*(\()(\s*)(?!"cid:)([^ )]*)(?=[ )])#i',
    );
    $body = preg_replace_callback($search, array(&$this, '_attach_regex'), $body);
    unset($this->_message);
  }

  private function _attach_regex($matches) {
    return
      ($path = drupal_realpath(DRUPAL_ROOT . parse_url($matches[4], PHP_URL_PATH)))
      && (is_readable($path) && !is_dir($path))
      && ($cid = $this->attach($this->_message, $path))
        ? $matches[1] . $matches[2] . "cid:$cid"
        : $matches[0];
  }
}
