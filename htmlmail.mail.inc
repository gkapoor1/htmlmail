<?php
/**
 * @file
 * Implementation of MailSystemInterface.
 * @see http://drupal.org/node/900794
 * @see http://api.drupal.org/api/drupal/includes--mail.inc/interface/MailSystemInterface/7
 */
class HTMLMailMailSystem implements MailSystemInterface {
  /**
   * Format emails according to module settings.
   */
  public function format(array $message) {
    // Collapse the message body array.
    $html_body = is_array($message['body'])
      ? implode("\n\n", $message['body'])
      : $message['body'];

    // Optionally apply a pre-filter.
    if ($filter = variable_get('htmlmail_prefilter')) {
      $html_body = check_markup($html_body, $filter);
    }

    // Add the configured header.
    if (($header = variable_get('htmlmail_header')) && $header['value']) {
      $html_body = $header['value'] . "\n\n" . $html_body;
    }

    // Add the configured footer.
    if (($footer = variable_get('htmlmail_footer')) && $footer['value']) {
      $html_body .= "\n\n" . $footer['value'];
    }

    // Create a plaintext equivalent, preferring the original if possible.
    if (($plain_body = strip_tags($html_body)) != $html_body) {
      $plain_body = drupal_html_to_text($html_body);
    }

    // Send the module name in email for theme template suggestions
    if (variable_get('htmlmail_debug', '0')) {
      $html_body .= '<p>Debug: module = ' . $message['module'] . '</p>';
      $plain_body .= "\n\nDebug: module = " . $message['module'];
    }

    // Skip formatting if the recipient prefers plaintext emails.
    $recipient = user_load_by_mail($message['to']);
    if ($recipient && !empty($recipient->data['htmlmail_plaintext'])) {
      $message['headers']['Content-Type'] = 'text/plain; charset=UTF-8; format=flowed';
      $message['body'] = $plain_body;
      return $message;
    }

    // Optionally apply the selected theme via internal http request.
    if ($theme = htmlmail_get_selected_theme($message['theme'])) {
      $url = url('admin/config/system/htmlmail/email', array('absolute' => TRUE));
      $data = 'body=' . rawurlencode($html_body)
        . '&subject=' . rawurlencode($message['subject'])
        . '&theme=' . rawurlencode($theme);
      $options = array(
        'method' => 'POST',
        'data' => $data,
        'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
      );
      $response = drupal_http_request($url, $options);
      $html_body = $response->data;
      // Strip javascript as it doesn't work through email anyway.
      $html_body = preg_replace('@<script type="text/javascript".*</script>@Usi', '', $html_body);
    }
    // If no theme was selected, use the htmlmail.tpl.php template.
    else {
      $html_body = theme('htmlmail', $message);
    }

    // Optionally replace css rules with inline styles.
    // @todo: move this to its own filter module.
    if (variable_get('htmlmail_emogrifier', '0') &&
      module_load_include('inc', 'htmlmail', 'emogrifier/emogrifier')) {
      $html_body = _htmlmail_emogrify($html_body);
    }

    // Optionally apply the selected post-filter.
    if ($filter = variable_get('htmlmail_postfilter')) {
      $html_body = check_markup($html_body, $filter);
    }

    // Store both plaintext and html versions.
    $boundary = uniqid(time(), 1);
    $message['headers']['Content-Type'] = "multipart/alternative; charset=utf-8; boundary=\"$boundary\"";
    $message['headers']['Content-Transfer-Encoding'] = '8bit';
    $message['headers']['MIME-Version'] = '1.0';
    $message['body'] = "This is a multi-part message in MIME format.
--$boundary
Content-Type: text/plain; charset=utf-8; format=flowed
Content-Transfer-Encoding: 8bit

$plain_body

--$boundary
Content-Type: text/html; charset=UTF-8; format=flowed
Content-Transfer-Encoding: 8bit

$html_body

--$boundary--
";
    return $message;
  }

  /**
   * Send an email message.
   */
  public function mail(array $message) {
    $mimeheaders = array();
    foreach ($message['headers'] as $name => $value) {
      $mimeheaders[] = $name . ': ' . mime_header_encode($value);
    }
    $line_endings = variable_get('mail_line_endings', MAIL_LINE_ENDINGS);
    $extra = '';
    if (isset($message['headers']['Return-Path']) &&
        !ini_get('safe_mode') &&
        !strpos(ini_get('sendmail_path'), ' -f')) {
      $extra = '-f ' . $message['headers']['Return-Path'];
    }

    return mail(
      $message['to'],
      mime_header_encode($message['subject']),
      // Note: email uses CRLF for line-endings. PHP's API requires LF
      // on Unix and CRLF on Windows. Drupal automatically guesses the
      // line-ending format appropriate for your system. If you need to
      // override this, adjust $conf['mail_line_endings'] in settings.php.
      preg_replace('@\r?\n@', $line_endings, $message['body']),
      // For headers, PHP's API suggests that we use CRLF normally,
      // but some MTAs incorrectly replace LF with CRLF. See #234403.
      join("\n", $mimeheaders),
      $extra
    );
  }
}
