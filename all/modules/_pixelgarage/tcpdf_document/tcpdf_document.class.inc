<?php

/**
 * @file
 * Contains \TCPDF\TCPDFDrupal.
 */

/**
 * The PDFDocument class further simplifies the creation of a PDF document with TCPDF. It contains additional methods to easily
 * create commonly used content, like document initialisation, header, footer, tables, images etc. and it centralizes the css for the pdf in a dedicated file,
 * which is loaded by default and can be adapted to the personal needs.
 *
 */
class PDFDocument extends TCPDF {

  protected $pdfHeader = array(
    'html' => NULL,
    'callback' => array(
      'function' => NULL,
      'context' => array(
        'textcolor' => array(0,0,0),
        'linecolor' => array(0,0,0),
      )
    )
  );

  protected $pdfFooter = array(
    'html' => NULL,
    'callback' => array(
      'function' => NULL,
      'context' => array(
        'textcolor' => array(0,0,0),
        'linecolor' => array(0,0,0),
      )
    )
  );

  /**
   * This method facilitates the construction of a PDF document drammatically by setting a bunch of options by default.
   *
   * It allows to set the document dependant properties of the PDFDocument object via input parameter and initializes the document with
   * a lot of basic document settings (margins, fonts, header, footer, image scaling etc.). This method also allows changing of the header
   * or footer of the pdf document.
   *
   * @param array $options
   *   Associative array containing basic settings. All values have a default.
   *     'title' => Title of the document
   *     'subject' => Subject of the document
   *     'author' => Author of the document
   *     'keywords' => Comma separated list of keywords
   *     'header' => Array
   *        'html' => Html code of the header.
   *        'callback' => array(              // Array of callback information
   *          'function' => NULL,             // Function that generates the header. If 'html' is set, it's ignored.
   *          'context' => array(             // Array of additional parameters for the callback function
   *            'textcolor' => array(0,0,0)   // Array with rgb-values for header text color, default = array(0,0,0)
   *            'linecolor' => array(0,0,0)   // Array with rgb-values for header line color, default = array(0,0,0)
   *          )
   *        )
   *     'footer' => Array
   *        'html' => Html code of the footer.
   *        'callback' => array(              // Array of callback information
   *          'function' => NULL,             // Function that generates the header. If 'html' is set, it's ignored.
   *          'context' => array(             // Array of additional parameters for the callback function
   *            'textcolor' => array(0,0,0)   // Array with rgb-values for header text color, default = array(0,0,0)
   *            'linecolor' => array(0,0,0)   // Array with rgb-values for header line color, default = array(0,0,0)
   *          )
   *        )
   *
   */
  function InitializeDocument($options = array()) {
    // default meta data
    $title = isset($options['title']) ? $options['title'] : variable_get('site_name', 'Title');
    $author = isset($options['author']) ? $options['author'] : variable_get('site_name', 'Drupal');
    $subject = isset($options['subject']) ? $options['subject'] : variable_get('site_name', 'Subject') ;
    $keywords = isset($options['keywords']) ? $options['keywords'] : 'pdf, Drupal, TCPDF, Pixelgarage';

    // define header / footer creation
    $options['header']['callback']['context'] = isset($options['header']['callback']['context']) ? array_merge($this->pdfHeader['callback']['context'], $options['header']['callback']['context']) : $this->pdfHeader['callback']['context'];
    $this->pdfHeader = isset($options['header']) ? array_merge($this->pdfHeader, $options['header']) : $this->pdfHeader;
    $options['footer']['callback']['context'] = isset($options['footer']['callback']['context']) ? array_merge($this->pdfFooter['callback']['context'], $options['footer']['callback']['context']) : $this->pdfFooter['callback']['context'];
    $this->pdfFooter = isset($options['footer']) ? array_merge($this->pdfFooter, $options['footer']) : $this->pdfFooter;

    // set document meta information
    $this->SetCreator(PDF_CREATOR);
    $this->SetAuthor($author);
    $this->SetTitle($title);
    $this->SetSubject($subject);
    $this->SetKeywords($keywords);

    // set default header / footer data
    $this->SetHeaderData(PDF_HEADER_LOGO, PDF_HEADER_LOGO_WIDTH, $title, $subject, $this->pdfHeader['callback']['context']['textcolor'], $this->pdfHeader['callback']['context']['linecolor']);
    $this->setFooterData($this->pdfFooter['callback']['context']['textcolor'], $this->pdfFooter['callback']['context']['linecolor']);

    // set default header and footer fonts
    $this->setHeaderFont(Array(PDF_FONT_NAME_MAIN, '', PDF_FONT_SIZE_MAIN));
    $this->setFooterFont(Array(PDF_FONT_NAME_DATA, '', PDF_FONT_SIZE_DATA));

    // set default monospaced font
    $this->SetDefaultMonospacedFont(PDF_FONT_MONOSPACED);

    // set document margins
    $this->SetMargins(PDF_MARGIN_LEFT, PDF_MARGIN_TOP, PDF_MARGIN_RIGHT);
    $this->SetHeaderMargin(PDF_MARGIN_HEADER);
    $this->SetFooterMargin(PDF_MARGIN_FOOTER);

    // set auto page breaks
    $this->SetAutoPageBreak(TRUE, PDF_MARGIN_BOTTOM);

    //set image scale factor
    $this->setImageScale(PDF_IMAGE_SCALE_RATIO);

    // set font
    $this->SetFont('dejavusans', '', 8);
    $this->AddPage();
  }

  /**
   * Sets the header of the document.
   * @return NULL
   */
  public function Header() {
    if (!$this->GenerateHeaderFooterSection($this->pdfHeader)) {
      parent::Header();
    }
  }

  /**
   * Sets the footer of the document.
   * @return NULL
   */
  public function Footer() {
    if (!$this->GenerateHeaderFooterSection($this->pdfFooter)) {
      parent::Footer();
    }
  }

  /**
   * Writes a drupal table render array with the given style to the pdf document.
   *
   * @param $table array  The render array of the table.
   * @param $x float x-Position of upper left corner in user units.
   * @param $y float y-Position of upper left corner in user units.
   * @param $style string The css properties for the table as a string. The string should not contain the <style> - tags. Default = '', means the table.css file is included.
   */
  public function DrupalTable($table, $x, $y, $style = '') {
    $tableHTML = $style;
    if (empty($style)) {
      $path = drupal_get_path('module', 'tcpdf_document') . '/css/table.css';
      $tableHTML = file_get_contents($path);
    }
    $tableHTML = "<style>" . $tableHTML . "</style>\n";

    // add table html
    $tableHTML .= drupal_render($table);

    // write the table
    $this->writeHTMLCell(0, 10, $x, $y, $tableHTML, 0, 1, false, true, '', true);
  }


  /**
   * Writes a drupal form (given by form id) with the given style to the pdf document.
   *
   * @param $form_id string Form ID of the form to be written to the pdf.
   * @param $x float x-Position of upper left corner in user units.
   * @param $y float y-Position of upper left corner in user units.
   * @param $w float width of the form in user units. $w = 0 extends form up to the right margin.
   * @param $editable bool Flag indicating if the form is editable in the pdf document. Default is true.
   * @param $hidden_classes array An array of css classes, which should be hidden on the form. This allows to hide specific form elements or regions. Default is an empty array.
   * @param $style string The css properties for the table as a string. The string should not contain the <style> - tags. Default = '', means the form.css file is included.
   */
  public function DrupalFormWithID($form_id, $x, $y, $w, $editable = true, $hidden_classes = array(), $style = '') {
    $form = drupal_get_form($form_id);
    $this->DrupalForm($form, $x, $y, $editable, $hidden_classes, $style);
  }


  /**
   * Writes a drupal form (given by form array) with the given style to the pdf document.
   *
   * @param $form array Form array of the form to be written to the pdf.
   * @param $x float x-Position of upper left corner in user units.
   * @param $y float y-Position of upper left corner in user units.
   * @param $w float width of the form in user units. $w = 0 extends form up to the right margin.
   * @param $editable bool Flag indicating if the form is editable in the pdf document. Default is true.
   * @param $hidden_elements array An array of form element id's, which should be hidden on the form. Default is an empty array.
   * @param $style string The css properties for the table as a string. The string should not contain the <style> - tags. Default = '', means the form.css file is included.
   */
  public function DrupalForm($form, $x, $y, $w, $editable = true, $hidden_elements = array(), $style = '') {
    $formHTML = $style;
    if (empty($style)) {
      $path = drupal_get_path('module', 'tcpdf_document') . '/css/form.css';
      $formHTML = file_get_contents($path);
    }
    $formHTML = "<style>" . $formHTML . "</style>\n";

    // hide form elements
    foreach ($hidden_elements as $elem) {
      hide($form[$elem]);
    }

    // add form html (editable or not)
    $formHTML .=  drupal_render($form);
    if (!$editable) {
      $formHTML = $this->replaceInputElementWithSpan($formHTML);
    }

    // write the table
    $this->writeHTMLCell($w, 10, $x, $y, $formHTML, 0, 1, false, true, '', true);
  }

  /**
   * Helper function to replace form elements with a span (not editable).
   *
   * @param $formHtml string Form as HTML string.
   * @return string The replaced HTML form string.
   */
  private function replaceInputElementWithSpan ($formHtml) {
    // replace textarea with span
    $formHtml = str_replace('<textarea', '<div', $formHtml);
    $formHtml = str_replace('</textarea>', '</div>', $formHtml);

    // find and replace <input value="value"/> with <span>value</span>
    $replaced_form = $formHtml;
    $form_parts = explode('<input', $formHtml);

    foreach ($form_parts as $i => $part) {
      if ($i == 0) {
        $replaced_form = $part;
        continue;
      }

      // find value for different kinds of input elements
      $replace = '';
      if (strpos($part, 'type="checkbox"') !== false) {
        // set value x or -
        $value = '☐';
        if (strpos($part, '="checked"') !== false) {
          $value = '☒';
        }
        $replace = '<span class="form-checkbox non-editable">' . $value . '</span>';

      } else if (strpos($part, 'type="text"') !== false) {
        $pos = strpos($part, 'value="');
        $part = substr($part, $pos+7);
        $pos = strpos($part, '"');
        $value = substr($part, 0, $pos);
        $replace = '<span class="form-text non-editable">' . $value . '</span>';

      } else if (strpos($part, 'type="hidden"') !== false) {
        // element is hidden, do nothing
      }

      // replace with span
      $pos = strpos($part, '/>');
      $replaced_form .= $replace . substr($part, $pos+2);

    }

    return $replaced_form;
  }

  /**
   * Generates a header or footer section for the pdf document.
   *
   * @param array $container
   * @see DrupalInitialize()
   *
   * @return FALSE if the container did not store any useful information to generate
   *   the document.
   */
  private function GenerateHeaderFooterSection($container) {
    if (!empty($container['html'])) {
      $this->writeHTML($container['html']);
      return TRUE;
    }
    elseif (!empty($container['callback'])) {
      $that = &$this;
      if (is_array($container['callback'])) {
        if (function_exists($container['callback']['function'])) {
          call_user_func($container['callback']['function'], $that, $container['callback']['context']);
        }
      }
      elseif (function_exists($container['callback'])) {
        call_user_func($container['callback'], $that, $container['callback']['context']);
      }
      return TRUE;
    }
    return FALSE;
  }

}
