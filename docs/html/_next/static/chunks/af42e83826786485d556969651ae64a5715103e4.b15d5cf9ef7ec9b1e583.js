(window.webpackJsonp_N_E=window.webpackJsonp_N_E||[]).push([[5],{"/0+H":function(e,t,n){"use strict";t.__esModule=!0,t.isInAmpMode=c,t.useAmp=function(){return c(o.default.useContext(a.AmpStateContext))};var r,o=(r=n("q1tI"))&&r.__esModule?r:{default:r},a=n("lwAK");function c(){var e=arguments.length>0&&void 0!==arguments[0]?arguments[0]:{},t=e.ampFirst,n=void 0!==t&&t,r=e.hybrid,o=void 0!==r&&r,a=e.hasQuery,c=void 0!==a&&a;return n||o&&c}},"2TiZ":function(e,t,n){e.exports={popup:"CqvAbxcdGSDF2kvHplFFD",popup_contentWrap:"_1AhCMaNJqZbdzPBRGDx2yy",popup_content:"_3BxhR41c11dCJRtPbXOPav",close:"bWnlJr9h9_9NQWHHcuWoG",popup_overlay:"_2KWTfEiRR1mmGXOCiAMWl8"}},"5fIB":function(e,t,n){var r=n("7eYB");e.exports=function(e){if(Array.isArray(e))return r(e)}},"8Kt/":function(e,t,n){"use strict";n("oI91");t.__esModule=!0,t.defaultHead=s,t.default=void 0;var r,o=function(e){if(e&&e.__esModule)return e;if(null===e||"object"!==typeof e&&"function"!==typeof e)return{default:e};var t=l();if(t&&t.has(e))return t.get(e);var n={},r=Object.defineProperty&&Object.getOwnPropertyDescriptor;for(var o in e)if(Object.prototype.hasOwnProperty.call(e,o)){var a=r?Object.getOwnPropertyDescriptor(e,o):null;a&&(a.get||a.set)?Object.defineProperty(n,o,a):n[o]=e[o]}n.default=e,t&&t.set(e,n);return n}(n("q1tI")),a=(r=n("Xuae"))&&r.__esModule?r:{default:r},c=n("lwAK"),u=n("FYa8"),i=n("/0+H");function l(){if("function"!==typeof WeakMap)return null;var e=new WeakMap;return l=function(){return e},e}function s(){var e=arguments.length>0&&void 0!==arguments[0]&&arguments[0],t=[o.default.createElement("meta",{charSet:"utf-8"})];return e||t.push(o.default.createElement("meta",{name:"viewport",content:"width=device-width"})),t}function p(e,t){return"string"===typeof t||"number"===typeof t?e:t.type===o.default.Fragment?e.concat(o.default.Children.toArray(t.props.children).reduce((function(e,t){return"string"===typeof t||"number"===typeof t?e:e.concat(t)}),[])):e.concat(t)}var f=["name","httpEquiv","charSet","itemProp"];function d(e,t){return e.reduce((function(e,t){var n=o.default.Children.toArray(t.props.children);return e.concat(n)}),[]).reduce(p,[]).reverse().concat(s(t.inAmpMode)).filter(function(){var e=new Set,t=new Set,n=new Set,r={};return function(o){var a=!0;if(o.key&&"number"!==typeof o.key&&o.key.indexOf("$")>0){var c=o.key.slice(o.key.indexOf("$")+1);e.has(c)?a=!1:e.add(c)}switch(o.type){case"title":case"base":t.has(o.type)?a=!1:t.add(o.type);break;case"meta":for(var u=0,i=f.length;u<i;u++){var l=f[u];if(o.props.hasOwnProperty(l))if("charSet"===l)n.has(l)?a=!1:n.add(l);else{var s=o.props[l],p=r[l]||new Set;p.has(s)?a=!1:(p.add(s),r[l]=p)}}}return a}}()).reverse().map((function(e,t){var n=e.key||t;return o.default.cloneElement(e,{key:n})}))}function y(e){var t=e.children,n=(0,o.useContext)(c.AmpStateContext),r=(0,o.useContext)(u.HeadManagerContext);return o.default.createElement(a.default,{reduceComponentsToState:d,headManager:r,inAmpMode:(0,i.isInAmpMode)(n)},t)}y.rewind=function(){};var h=y;t.default=h},FYa8:function(e,t,n){"use strict";var r;t.__esModule=!0,t.HeadManagerContext=void 0;var o=((r=n("q1tI"))&&r.__esModule?r:{default:r}).default.createContext({});t.HeadManagerContext=o},Rp8q:function(e,t,n){e.exports={title:"wZV8vEkiUelVZfsJqjeN5",center:"_2cQlf5PsXRhtvc3DtRyO36"}},T1a2:function(e,t,n){"use strict";var r=n("1OyB"),o=n("vuIU"),a=n("Ji7U"),c=n("md7G"),u=n("foSv"),i=n("q1tI"),l=n.n(i),s=n("nD/M"),p=n.n(s),f=l.a.createElement;function d(e){var t=function(){if("undefined"===typeof Reflect||!Reflect.construct)return!1;if(Reflect.construct.sham)return!1;if("function"===typeof Proxy)return!0;try{return Date.prototype.toString.call(Reflect.construct(Date,[],(function(){}))),!0}catch(e){return!1}}();return function(){var n,r=Object(u.a)(e);if(t){var o=Object(u.a)(this).constructor;n=Reflect.construct(r,arguments,o)}else n=r.apply(this,arguments);return Object(c.a)(this,n)}}var y=function(e){Object(a.a)(n,e);var t=d(n);function n(){return Object(r.a)(this,n),t.apply(this,arguments)}return Object(o.a)(n,[{key:"render",value:function(){return f("small",{className:p.a.notes},this.props.content)}}]),n}(i.Component);t.a=y},U7NL:function(e,t,n){"use strict";var r=n("1OyB"),o=n("vuIU"),a=n("Ji7U"),c=n("md7G"),u=n("foSv"),i=n("q1tI"),l=n.n(i),s=n("Qetd"),p=n.n(s),f=function(){return(f=Object.assign||function(e){for(var t,n=1,r=arguments.length;n<r;n++)for(var o in t=arguments[n])Object.prototype.hasOwnProperty.call(t,o)&&(e[o]=t[o]);return e}).apply(this,arguments)},d=function(e){for(var t=e.url,n=e.allowFullScreen,r=e.position,o=e.display,a=e.height,c=e.width,u=e.overflow,i=e.styles,s=e.onLoad,d=e.onMouseOver,y=e.onMouseOut,h=e.scrolling,v=e.id,m=e.frameBorder,b=e.ariaHidden,g=e.sandbox,O=e.allow,w=e.className,_=e.title,j=e.ariaLabel,M=e.ariaLabelledby,R=e.name,x=e.target,C=e.loading,k=e.importance,S=e.referrerpolicy,I=e.allowpaymentrequest,P=e.src,D=p()({src:P||t,target:x||null,style:{position:r||null,display:o||"block",overflow:u||null},scrolling:h||null,allowpaymentrequest:I||null,importance:k||null,sandbox:g||null,loading:C||null,styles:i||null,name:R||null,className:w||null,referrerpolicy:S||null,title:_||null,allow:O||null,id:v||null,"aria-labelledby":M||null,"aria-hidden":b||null,"aria-label":j||null,width:c||null,height:a||null,onLoad:s||null,onMouseOver:d||null,onMouseOut:y||null}),A=Object.create(null),H=0,q=Object.keys(D);H<q.length;H++){var N=q[H];null!=D[N]&&(A[N]=D[N])}for(var E=0,W=Object.keys(A.style);E<W.length;E++){var U=W[E];null==A.style[U]&&delete A.style[U]}if(n)if("allow"in A){var B=A.allow.replace("fullscreen","");A.allow=("fullscreen "+B.trim()).trim()}else A.allow="fullscreen";return m>=0&&(A.style.hasOwnProperty("border")||(A.style.border=m)),l.a.createElement("iframe",f({},A))},y=l.a.createElement;function h(e){var t=function(){if("undefined"===typeof Reflect||!Reflect.construct)return!1;if(Reflect.construct.sham)return!1;if("function"===typeof Proxy)return!0;try{return Date.prototype.toString.call(Reflect.construct(Date,[],(function(){}))),!0}catch(e){return!1}}();return function(){var n,r=Object(u.a)(e);if(t){var o=Object(u.a)(this).constructor;n=Reflect.construct(r,arguments,o)}else n=r.apply(this,arguments);return Object(c.a)(this,n)}}var v=function(e){Object(a.a)(n,e);var t=h(n);function n(){return Object(r.a)(this,n),t.apply(this,arguments)}return Object(o.a)(n,[{key:"render",value:function(){return y(d,{width:"854",height:"480",url:this.props.url,allow:"accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture,fullscreen"})}}]),n}(i.Component),m=n("2TiZ"),b=n.n(m),g=l.a.createElement;function O(e){var t=function(){if("undefined"===typeof Reflect||!Reflect.construct)return!1;if(Reflect.construct.sham)return!1;if("function"===typeof Proxy)return!0;try{return Date.prototype.toString.call(Reflect.construct(Date,[],(function(){}))),!0}catch(e){return!1}}();return function(){var n,r=Object(u.a)(e);if(t){var o=Object(u.a)(this).constructor;n=Reflect.construct(r,arguments,o)}else n=r.apply(this,arguments);return Object(c.a)(this,n)}}var w=function(e){Object(a.a)(n,e);var t=O(n);function n(){return Object(r.a)(this,n),t.apply(this,arguments)}return Object(o.a)(n,[{key:"render",value:function(){var e=this;return g("div",{className:b.a.popup},g("div",{className:b.a.popup_contentWrap},g("div",{className:b.a.popup_content},g("button",{type:"button",className:b.a.close,onClick:function(){return e.props.closePopup(e.props.url)}},g("img",{src:"/static/images/icon/closepopup.svg",alt:""})),g(v,{url:this.props.url}))),g("div",{className:b.a.popup_overlay,role:"button",onClick:function(){return e.props.closePopup(e.props.url)}}))}}]),n}(i.Component);t.a=w},Xuae:function(e,t,n){"use strict";var r=n("mPvQ"),o=n("/GRZ"),a=n("i2R6"),c=(n("qXWd"),n("48fX")),u=n("tCBg"),i=n("T0f4");function l(e){var t=function(){if("undefined"===typeof Reflect||!Reflect.construct)return!1;if(Reflect.construct.sham)return!1;if("function"===typeof Proxy)return!0;try{return Date.prototype.toString.call(Reflect.construct(Date,[],(function(){}))),!0}catch(e){return!1}}();return function(){var n,r=i(e);if(t){var o=i(this).constructor;n=Reflect.construct(r,arguments,o)}else n=r.apply(this,arguments);return u(this,n)}}t.__esModule=!0,t.default=void 0;var s=n("q1tI"),p=function(e){c(n,e);var t=l(n);function n(e){var a;return o(this,n),(a=t.call(this,e))._hasHeadManager=void 0,a.emitChange=function(){a._hasHeadManager&&a.props.headManager.updateHead(a.props.reduceComponentsToState(r(a.props.headManager.mountedInstances),a.props))},a._hasHeadManager=a.props.headManager&&a.props.headManager.mountedInstances,a}return a(n,[{key:"componentDidMount",value:function(){this._hasHeadManager&&this.props.headManager.mountedInstances.add(this),this.emitChange()}},{key:"componentDidUpdate",value:function(){this.emitChange()}},{key:"componentWillUnmount",value:function(){this._hasHeadManager&&this.props.headManager.mountedInstances.delete(this),this.emitChange()}},{key:"render",value:function(){return null}}]),n}(s.Component);t.default=p},"ZS+n":function(e,t,n){"use strict";var r=n("q1tI"),o=n.n(r),a=n("8Kt/"),c=n.n(a),u=n("NQWt"),i=o.a.createElement;t.a=function(e){var t=e.title,n=e.description,r=e.ogDescription,o=e.ogUrl,a=e.ogImg,l=e.dir,s=l?"".concat(u.a).concat(l.replace(/^\//,""),"/"):u.a;return i(c.a,null,i("meta",{name:"viewport",content:"width=device-width,initial-scale=1.0"}),i("title",null,t),i("meta",{name:"description",content:n}),i("meta",{property:"og:locale",content:"jp_JP"}),i("meta",{property:"og:type",content:"website"}),i("meta",{property:"og:title",content:t}),i("meta",{property:"og:description",content:"undefined"===typeof r?n:r}),i("meta",{property:"og:url",content:"undefined"===typeof o?u.a:o}),i("meta",{property:"og:site_name",content:"42 Tokyo"}),i("meta",{property:"og:image",content:"undefined"===typeof a||""===a?"".concat(u.a,"static/images/42ogp_jp.jpg"):a}),i("link",{rel:"icon",type:"image/x-icon",href:"/favicon.ico"}),i("link",{rel:"canonical",href:s}),i("script",{src:"https://www.googleoptimize.com/optimize.js?id=OPT-MF6WJN4"}))}},kG2m:function(e,t){e.exports=function(){throw new TypeError("Invalid attempt to spread non-iterable instance.\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.")}},lwAK:function(e,t,n){"use strict";var r;t.__esModule=!0,t.AmpStateContext=void 0;var o=((r=n("q1tI"))&&r.__esModule?r:{default:r}).default.createContext({});t.AmpStateContext=o},mPvQ:function(e,t,n){var r=n("5fIB"),o=n("rlHP"),a=n("KckH"),c=n("kG2m");e.exports=function(e){return r(e)||o(e)||a(e)||c()}},"nD/M":function(e,t,n){e.exports={notes:"_1prVQyD5sBeKORhU-3aMgj",bld_notes:"_1hO5IH2rSM_Iq2Aozipv5X"}},oI91:function(e,t){e.exports=function(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}},rlHP:function(e,t){e.exports=function(e){if("undefined"!==typeof Symbol&&Symbol.iterator in Object(e))return Array.from(e)}},tHSz:function(e,t,n){"use strict";var r=n("1OyB"),o=n("vuIU"),a=n("Ji7U"),c=n("md7G"),u=n("foSv"),i=n("q1tI"),l=n.n(i),s=n("Rp8q"),p=n.n(s),f=l.a.createElement;function d(e){var t=function(){if("undefined"===typeof Reflect||!Reflect.construct)return!1;if(Reflect.construct.sham)return!1;if("function"===typeof Proxy)return!0;try{return Date.prototype.toString.call(Reflect.construct(Date,[],(function(){}))),!0}catch(e){return!1}}();return function(){var n,r=Object(u.a)(e);if(t){var o=Object(u.a)(this).constructor;n=Reflect.construct(r,arguments,o)}else n=r.apply(this,arguments);return Object(c.a)(this,n)}}var y=function(e){Object(a.a)(n,e);var t=d(n);function n(){return Object(r.a)(this,n),t.apply(this,arguments)}return Object(o.a)(n,[{key:"render",value:function(){var e="".concat(p.a.title," ").concat(this.props.center?p.a.center:""," ").concat(this.props.cssClass?this.props.cssClass:"");return f("div",{className:e},"h1"===this.props.type&&f("h1",null,this.props.text),"h1"!==this.props.type&&f("h2",null,this.props.text))}}]),n}(i.Component);t.a=y}}]);