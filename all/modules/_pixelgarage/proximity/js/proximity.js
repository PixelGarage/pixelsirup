/**
 * ---
 * @author Ralph Moser (http://ramosoft.ch)
 * @version 0.1
 * @updated 11-06-14
 * ---
 */

(function($) {

    /**
     * This behavior adds the proximity event to all items in the responsive proximity widget.
     * The proximity event is supported in IE 9+, Chrome, Firefox, Safari and Opera.
     */
    Drupal.behaviors.proximityViews =  {
        attach: function() {

          // Iterate through all proximity view instances
          $.each(Drupal.settings.proximity, function (container, settings) {

            var $container  = $(container),
                $items      = $container.find(settings.item_selector),
                minScale    = parseFloat(settings.start_scale),
                maxScale	= parseFloat(settings.end_scale),
                minOpacity	= parseFloat(settings.start_opacity),
                maxOpacity	= parseFloat(settings.end_opacity),
                eventData   = {
                    min             : 0,
                    max             : parseInt(settings.extent),
                    throttle        : 20,
                    fireOutOfBounds : true
                },
                _calcItemDesc = function($item) {
                    var $desc	= (settings.desc_selector) ? $container.find(settings.desc_selector) : $([]);
                    if ( $desc.length == 0 ) return $desc;

                    // if description is available, size and position the description container based on the scaled cell
                    var wCell       = $item.width(),
                        hCell       = $item.height(),
                        swCell	    = maxScale * wCell,
                        shCell	    = maxScale * hCell,
                        topShift    = ( shCell - hCell ) / 2,
                        leftShift   = ( swCell - wCell ) / 2,
                        // show cell description always inside of the cell container (left or right of cell)
                        left        = ( $item.offset().left + wCell + $desc.width() > $container.offset().left + $container.width() )
                                        ? -$desc.width() - leftShift    // left of cell
                                        : swCell - leftShift;           // right of cell
                    // size and position description
                    return $desc.css({
                                height	: Math.max($desc.height(), shCell),
                                top		: -topShift,
                                left	: left
                    });

                };

            // attach the proximity event handler to all cells (once)
            $items.once('pe', function() {

                $(this).on('proximity', eventData, function(event, proximity, distance) {

                    var $item		= $(this),
                        $desc       = $item.next(),
                        scaleVal	= proximity * ( maxScale - minScale ) + minScale,
                        scaleExp	= 'scale(' + scaleVal + ')',
                        opacityVal  = proximity * ( maxOpacity - minOpacity ) + minOpacity;

                    // force the cell to the front when it reaches the maximum scale value and show its description, if available
                    if (scaleVal === maxScale) {

                        // put cell to front
                        $item.css( 'z-index', 10 );

                        // fade in cell description correctly sized and positioned (left or right of cell)
                        _calcItemDesc($item).fadeIn(800);

                    } else {

                        // reset cell, stop animation and hide description
                        $item.css( 'z-index', 1 );
                        //$desc.stop(true,true).hide();

                    };

                    // scale cell and set its transparency
                    $item.css({
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

          }); // container instances

        }
    };

})(jQuery);

