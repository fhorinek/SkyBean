
chrome.app.runtime.onLaunched.addListener(function() {


  chrome.app.window.create('index.html', {
    id: "MainWindowID",
    frame: 'chrome',
    innerBounds: {
      minWidth: 900,
      minHeight: 500,
      }
  });
});
