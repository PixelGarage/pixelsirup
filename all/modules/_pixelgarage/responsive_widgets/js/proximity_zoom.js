/**
 * ---
 * @author Ralph Moser (http://ramosoft.ch)
 * @version 0.1
 * @updated 07-Dec-13
 * ---
 */

(function($) {

    /**
     * This behavior adds the proximity event to all cells in the responsive proximity widget container.
     * The proximity event is supported in IE 9+, Chrome, Firefox, Safari and Opera.
     *
     * @type {{attach: Function}}
     */
    Drupal.behaviors.rwProximityZoom =  {
        attach: function() {

            // get the cells and define global settings and event data
            var $container  = $('#pe-cell-container'),
                $cells      = $container.find('.cell'),
                settings    = {
                    minScale    : parseFloat(Drupal.settings.rwProximity.startScale),
                    maxScale	: parseFloat(Drupal.settings.rwProximity.endScale),
                    minOpacity	: parseFloat(Drupal.settings.rwProximity.startOpacity),
                    maxOpacity	: parseFloat(Drupal.settings.rwProximity.endOpacity)
                },
                eventData   = {
                    min             : 0,
                    max             : parseInt(Drupal.settings.rwProximity.extent),
                    throttle        : 20,
                    fireOutOfBounds : true
                };

            var _calcCellDesc = function($cell) {

                var $desc	= $cell.next();
                if ( $desc.length == 0 ) return;

                // if description is available, size and position the description container based on the scaled cell
                var wCell	    = $cell.width(),
                    // the cell has no content height, only its parent has (responsive design)
                    hCell	    = $cell.parent().height(),
                    swCell	    = settings.maxScale * wCell,
                    shCell	    = settings.maxScale * hCell,
                    topShift    = ( shCell - hCell ) / 2,
                    leftShift   = ( swCell - wCell ) / 2,
                    // show cell description always inside of the cell container (left or right of cell)
                    left        = ( $cell.offset().left + wCell + $desc.width() > $container.offset().left + $container.width() )
                                    ? -$desc.width() - leftShift    // left of cell
                                    : swCell - leftShift;           // right of cell

                $desc.css({
                    height	: Math.max($desc.height(), shCell),
                    top		: -topShift,
                    left	: left
                });

            };

            // attach the proximity event handler to all cells (once)
            $cells.once('pe', function() {

                $(this).on('proximity.Zoom', eventData, function(event, proximity, distance) {

                    var $cell		= $(this),
                        $desc       = $cell.next(),
                        scaleVal	= proximity * ( settings.maxScale - settings.minScale ) + settings.minScale,
                        scaleExp	= 'scale(' + scaleVal + ')',
                        opacityVal  = proximity * ( settings.maxOpacity - settings.minOpacity ) + settings.minOpacity;

                    // force the cell to the front when it reaches the maximum scale value and show its description, if available
                    if (scaleVal === settings.maxScale) {

                        // put cell to front
                        $cell.css( 'z-index', 10 );

                        // fade in cell description correctly sized and positioned (left or right of cell)
                        _calcCellDesc($cell);
                        $desc.fadeIn( 800 );

                    } else {

                        // reset cell, stop animation and hide description
                        $cell.css( 'z-index', 1 );
                        $desc.stop(true,true).hide();

                    };

                    // scale cell and set its transparency
                    $cell.css({
                        '-webkit-transform'	: scaleExp,
                        '-moz-transform'	: scaleExp,
                        '-o-transform'		: scaleExp,
                        '-ms-transform'		: scaleExp,
                        'transform'			: scaleExp,
                        'opacity'			: opacityVal
                    });

                });

            });

            // init all cells with given settings by triggering a mouse move on document
            $(document).trigger('mousemove');

        }
    };

})(jQuery);

