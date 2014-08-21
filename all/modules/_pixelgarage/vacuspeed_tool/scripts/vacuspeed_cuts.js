/**
 * Defines behaviors to control the cuts definition.
 *
 * @author Ralph Moser (http://ramosoft.ch)
 * @version 0.1
 * @updated 8-Aug-2014
 * ---
 */

(function($) {

    Drupal.modalDialog = Drupal.modalDialog || {};

    /**
     * Position the add buttons around the image.
     *
     * @type {{attach: Function}}
     */
    Drupal.behaviors.positionAddButtons =  {
        attach: function() {
            var $addButtons = $("#tiled-image-wrapper .add-button"),
                $image = $('#tiled-image-wrapper .tiled-image-0 > img'),
                _positionButtons = function() {
                    var width = $image.width(),
                        height = $image.height(),
                        shift = -16;

                    $addButtons.each(function() {
                        var $this = $(this);

                        if ($this.hasClass('add-button-0')) {
                            // top left
                            $this.css({"cursor": "pointer", "position": "absolute", "left": shift, "top": shift, "width": "32px", "height": "32px"})
                        } else if ($this.hasClass('add-button-1')) {
                            // top middle
                            $this.css({"cursor": "pointer", "position": "absolute", "left": (width/2 + shift), "top": shift, "width": "32px", "height": "32px"})
                        } else if ($this.hasClass('add-button-2')) {
                            // top right
                            $this.css({"cursor": "pointer", "position": "absolute", "left": (width + shift), "top": shift, "width": "32px", "height": "32px"})
                        } else if ($this.hasClass('add-button-3')) {
                            // middle left
                            $this.css({"cursor": "pointer", "position": "absolute", "left": shift, "top": (height/2 + shift), "width": "32px", "height": "32px"})
                        } else if ($this.hasClass('add-button-4')) {
                            // middle middle
                            $this.css({"cursor": "pointer", "position": "absolute", "left": (width/2 + shift), "top": (height/2 + shift), "width": "32px", "height": "32px"})
                        } else if ($this.hasClass('add-button-5')) {
                            // middle right
                            $this.css({"cursor": "pointer", "position": "absolute", "left": (width + shift), "top": (height/2 + shift), "width": "32px", "height": "32px"})
                        } else if ($this.hasClass('add-button-6')) {
                            // bottom left
                            $this.css({"cursor": "pointer", "position": "absolute", "left": shift, "top": (height + shift), "width": "32px", "height": "32px"})
                        } else if ($this.hasClass('add-button-7')) {
                            // bottom middle
                            $this.css({"cursor": "pointer", "position": "absolute", "left": (width/2 + shift), "top": (height + shift), "width": "32px", "height": "32px"})
                        } else if ($this.hasClass('add-button-8')) {
                            // bottom right
                            $this.css({"cursor": "pointer", "position": "absolute", "left": (width + shift), "top": (height + shift), "width": "32px", "height": "32px"})
                        }

                    });

                };

            // position all buttons around image
            $image.on('load', _positionButtons);
            $(window).resize(_positionButtons);
        }

    }

    /**
     * Defines a dialog with a containing form.
     *
     * @type {{attach: Function}}
     */
    Drupal.behaviors.cutDefinitionPopup =  {
        attach: function() {
            // create a dialog with the cut form
            var $form = $("#vacuspeed-tool-cut-form"),
                $addButtons = $("#tiled-image-wrapper .add-button"),
                settings = Drupal.settings.vacuspeed_tool;

            // create a hidden dialog with the cut definition form (move submit button out of view)

            Drupal.modalDialog = $form.dialog({
                title: settings.dialog_title,
                autoOpen: false,
                width: 300,
                modal: true,
                resizable: false,
                dialogClass: "no-close",
                show: {
                    effects: "fade",
                    duration: 200
                },
                hide: {
                    effects: "fade",
                    duration: 200
                }
            });

            // open the dialog in correspondence to the clicked add-button
            $addButtons.once('binding', function() {
                // bind click event to all add buttons
                $addButtons.on('click', function() {
                    // reset and enable all form elements
                    var $this = $(this),
                        $left = $form.find('.text-left'),
                        $top = $form.find('.text-top'),
                        $helperImg = $form.find('.form-helper-image img'),
                        helperImgSrc = $helperImg.attr('src'),
                        res = helperImgSrc.substring(0, helperImgSrc.length -6);
                    $left.val('0');
                    $top.val('0');
                    $form.find('.text-width').val('0');
                    $form.find('.text-height').val('0');
                    $form.find('.form-item-vacuspeed-cut-left, .form-item-vacuspeed-cut-top, .form-item-vacuspeed-cut-width, .form-item-vacuspeed-cut-height').show();

                    // disable not relevant form elements and set -1 for values to be calculated on the server
                    if ($this.hasClass('add-button-0')) {
                        // top left
                        res += 'tl.gif';
                        $form.find('.form-item-vacuspeed-cut-left, .form-item-vacuspeed-cut-top').hide();
                    } else if ($this.hasClass('add-button-1')) {
                        // top middle
                        res += 'tm.gif';
                        $form.find('.form-item-vacuspeed-cut-top').hide();
                    } else if ($this.hasClass('add-button-2')) {
                        // top right
                        res += 'tr.gif';
                        $left.val('-1');
                        $form.find('.form-item-vacuspeed-cut-left, .form-item-vacuspeed-cut-top').hide();
                    } else if ($this.hasClass('add-button-3')) {
                        // middle left
                        res += 'ml.gif';
                        $form.find('.form-item-vacuspeed-cut-left').hide();
                    } else if ($this.hasClass('add-button-4')) {
                        // middle middle
                        res += 'mm.gif';
                    } else if ($this.hasClass('add-button-5')) {
                        // middle right
                        res += 'mr.gif';
                        $left.val('-1');
                        $form.find('.form-item-vacuspeed-cut-left').hide();
                    } else if ($this.hasClass('add-button-6')) {
                        // bottom left
                        res += 'bl.gif';
                        $top.val('-1');
                        $form.find('.form-item-vacuspeed-cut-left, .form-item-vacuspeed-cut-top').hide();
                    } else if ($this.hasClass('add-button-7')) {
                        // bottom middle
                        res += 'bm.gif';
                        $top.val('-1');
                        $form.find('.form-item-vacuspeed-cut-top').hide();
                    } else if ($this.hasClass('add-button-8')) {
                        // bottom right
                        res += 'br.gif';
                        $left.val('-1');
                        $top.val('-1');
                        $form.find('.form-item-vacuspeed-cut-left, .form-item-vacuspeed-cut-top').hide();
                    }
                    $helperImg.attr('src', res);

                    // open the dialog
                    Drupal.modalDialog.dialog( "open" );
                });
            });

        }
    }

    /**
     * Ajax command to close the dialog.
     */
    Drupal.ajax.prototype.commands.dialog_close = function(ajax, response, status) {
        var element = Drupal.modalDialog;

        // Process any other behaviors on the content, and close the dialog box.
        Drupal.detachBehaviors(element);
        element.dialog('close');
    };

})(jQuery);



