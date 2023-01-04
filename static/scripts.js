const animateCSS = (element, animation, prefix = 'animate__') =>
    // We create a Promise and return it
    new Promise((resolve, reject) => {
        const animationName = `${prefix}${animation}`;
        const node = document.querySelector(element);

        node.classList.add(`${prefix}animated`, animationName);

        // When the animation ends, we clean the classes and resolve the Promise
        function handleAnimationEnd(event) {
            event.stopPropagation();
            node.classList.remove(`${prefix}animated`, animationName);
            resolve('Animation ended');
        }

        node.addEventListener('animationend', handleAnimationEnd, { once: true });
    });

$(document).ready(function () {
    $("#button-right").click(function () {
        $("#button-right").click(fadeOutElements())

    });
});
function fadeOutElements() {

    animateCSS("#button-left", "fadeOutLeft").then((message) => {
        $("#button-left").css('visibility', 'hidden')
    });
    setTimeout(function () {
        animateCSS(".content", "fadeOutLeft").then((message) => {
            $(".content").css('visibility', 'hidden')
            $(".content").load('/static/p2.html')

        });
    }, 300);
    setTimeout(function () {
        animateCSS("#button-right", "fadeOutLeft").then((message) => {
            $("#button-right").css('visibility', 'hidden');
            console.log("hi")
        });;
    }, 400);
};

