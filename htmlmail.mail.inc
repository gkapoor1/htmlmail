<?php
/**
 * @file
 * Implements MailSystemInterface.
 *
 * Declares the HTMLMailSystem class, which formats and sends mail using the
 * MailMIME class supplied by the Mail MIME module.
 *
 * @see http://drupal.org/node/900794
 * @see http://api.drupal.org/api/drupal/includes--mail.inc/interface/MailSystemInterface/7
 * @see http://drupal.org/project/mailmime
 */

module_load_include('inc','mailmime');

/**
 * Implements MailSystemInterface.
 *
 * Also supplies helper functions for manipulating email messages.
 */
class HTMLMailSystem implements MailSystemInterface {
  /**
   * Format emails according to module settings.
   *
   * Parses the message headers and body into a MailMIME object.  If another module
   * subsequently modifies the body, then format() should be called again before
   * sending.  This is safe because the $message['body'] is not modified.
   *
   * @param $message
   *   An associative array with at least the following parts:
   *   - headers: An array of (name => value) email headers.
   *   - body: The text/plain or text/html message part.
   *
   * @return
   *   The formatted $message, ready for sending.
   */
  public function format(array $message) {
    // Build a full email message string.
    $email = MailMIME::encodeEmail($message['headers'], $message['body']);
    // Parse it into MIME parts.
    if (!($mime = MailMIME::parse($email))) {
      // Logged unparseable by MailMIME::parse(). What else to do?
      $message = array('headers' => array(), 'body' => '') + $message;
      return $message;
    }
    // Extract the message body.
    if ( !($body = $mime->getHTMLBody()) // Prefer the text/html part.
      && !($body = $mime->getTXTBody()) ) { // Fall back to text/plain.
      $body = ''; // Or a blank string if necessary.
    }
    // Work on a copy so that the original $message['body'] remains unchanged.
    $email = $message;
    list($email['headers'], $email['body']) = $mime->toEmail($email['headers']);
    // Theme with htmlmail.tpl.php.
    $body = theme('htmlmail', $email);
    // Store the plaintext version.
    $mime->setTXTBody(drupal_html_to_text($body));
    // Check to see whether recipient allows non-plaintext.
    if ( !($recipient = user_load_by_mail($message['to']))
      || empty($recipient->data['htmlmail_plaintext']) ) {
      // Optionally apply the selected web theme.
      if ($theme = htmlmail_get_selected_theme($message)) {
        $body = echo_themed_page($message['subject'], $body, $theme);
      }
      // Optionally apply the selected output filter.
      if ($filter = variable_get('htmlmail_postfilter')) {
        $body = check_markup($body, $filter);
      }
      // Store the fully-themed HTML body.
      $mime->setHTMLBody($body);
    }
    // Store the MailMIME object.
    $message['MailMIME'] = &$mime;
    return $message;
  }

  /**
   * Send an email message.
   *
   * @param $message
   *   An associative array containing at least:
   *   - headers: An associative array of (name => value) email headers.
   *   - body: The text/plain or text/html message body.
   *   - MailMIME: The message, parsed into a MailMIME object.
   */
  public function mail(array $message) {
    // If message was not parsed into a MailMIME object, abort.
    if (!isset($message['MailMIME'])) {
      return FALSE;
    }
    $mime = &$message['MailMIME'];
    // Extract headers and body parts.
    list($headers, $body) = $mime->toEmail($message['headers']);
    // Ensure that subject is non-null.
    $message += array('subject' => t('(No subject)'));
    // Check for empty recipient.
    if (empty($message['to'])) {
      if (empty($headers['To'])) {
        watchdog(
          'HTMLMailSystem',
          'Cannot send email about %subject without a recipient.',
          array('subject' => $message['subject']),
          WATCHDOG_ERROR
        );
        return FALSE;
      }
      $message['to'] = $headers['To'];
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
      MailMIME::encodeHeader('to', $message['to'], 'UTF-8'),
      MailMIME::encodeHeader('subject', $message['subject'], 'UTF-8'),
      $body,
      $headers,
      $extra
    );
  }
}
