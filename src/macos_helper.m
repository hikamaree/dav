#import <Cocoa/Cocoa.h>
#include <gdk/gdk.h>
#include "data.h"
#include "audio.h"
#include "config.h"
#include "gif.h"
#include "ui.h"

#ifdef GDK_WINDOWING_QUARTZ
#include <gdk/gdkquartz.h>
#include <gdk/quartz/gdkquartz-cocoa-access.h>
#endif

void macos_set_window_on_all_spaces(GdkWindow *gdk_window) {
    if (!gdk_window) return;
    
#ifdef GDK_WINDOWING_QUARTZ
    NSView *view = gdk_quartz_window_get_nsview(gdk_window);
    if (view) {
        NSWindow *ns_window = [view window];
        if (ns_window) {
            // Set window to appear on all Spaces/Desktops
            [ns_window setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces | 
                                              NSWindowCollectionBehaviorStationary |
                                              NSWindowCollectionBehaviorFullScreenAuxiliary];
            
            // Keep window above normal windows but below floating panels
            [ns_window setLevel:NSStatusWindowLevel];
            
            // Overlay must ignore mouse events (pass-through for clicks)
            [ns_window setIgnoresMouseEvents:YES];
        }
    }
#endif
}

@interface SettingsWindowController : NSWindowController <NSWindowDelegate>
@property (nonatomic, assign) AppData* appData;
@property (nonatomic, strong) NSPopUpButton* devicePopup;
@property (nonatomic, strong) NSButton* startStopButton;
@property (nonatomic, strong) NSTextField* deviceLabel;
@property (nonatomic, strong) NSSlider* radiusSlider;
@property (nonatomic, strong) NSSlider* spaceSlider;
@property (nonatomic, strong) NSSlider* speedSlider;
@property (nonatomic, strong) NSTextField* redField;
@property (nonatomic, strong) NSTextField* greenField;
@property (nonatomic, strong) NSTextField* blueField;
@property (nonatomic, strong) NSTextField* alphaField;
@property (nonatomic, strong) NSTabView* mainTabView;
@property (nonatomic, strong) NSTabView* gifsTabView;
@property (nonatomic, strong) NSView* settingsTab;
@property (nonatomic, strong) NSView* gifsTab;
- (void)refreshGifTabs;
- (void)addGifTab:(Gif*)gif;
@end

@implementation SettingsWindowController

- (instancetype)initWithAppData:(AppData*)data {
    NSWindow *window = [[NSWindow alloc] initWithContentRect:NSMakeRect(100, 100, 700, 550)
                                                    styleMask:(NSWindowStyleMaskTitled | 
                                                              NSWindowStyleMaskClosable | 
                                                              NSWindowStyleMaskMiniaturizable |
                                                              NSWindowStyleMaskResizable)
                                                      backing:NSBackingStoreBuffered
                                                        defer:NO];
    
    self = [super initWithWindow:window];
    if (self) {
        self.appData = data;
        [window setTitle:@"Directional Audio Visualizer"];
        [window setDelegate:self];
        [self setupUI];
    }
    return self;
}

- (void)setupUI {
    NSView *contentView = self.window.contentView;
    
    // Create main tab view
    self.mainTabView = [[NSTabView alloc] initWithFrame:NSMakeRect(0, 0, 700, 550)];
    [contentView addSubview:self.mainTabView];
    
    // Settings tab
    self.settingsTab = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 680, 500)];
    NSTabViewItem *settingsItem = [[NSTabViewItem alloc] initWithIdentifier:@"settings"];
    [settingsItem setLabel:@"Visualizer"];
    [settingsItem setView:self.settingsTab];
    [self.mainTabView addTabViewItem:settingsItem];
    
    // GIFs container tab
    self.gifsTab = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 680, 500)];
    NSTabViewItem *gifsItem = [[NSTabViewItem alloc] initWithIdentifier:@"gifs"];
    [gifsItem setLabel:@"GIFs"];
    [gifsItem setView:self.gifsTab];
    [self.mainTabView addTabViewItem:gifsItem];
    
    [self setupSettingsTab];
    [self setupGifsTab];
}

- (void)setupSettingsTab {
    NSView *view = self.settingsTab;
    
    // Device label at top
    self.deviceLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 450, 640, 24)];
    [self.deviceLabel setStringValue:@""];
    [self.deviceLabel setBezeled:NO];
    [self.deviceLabel setDrawsBackground:NO];
    [self.deviceLabel setEditable:NO];
    [self.deviceLabel setAlignment:NSTextAlignmentCenter];
    [view addSubview:self.deviceLabel];
    
    // Device popup
    self.devicePopup = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(20, 410, 200, 26)];
    [self.devicePopup setTarget:self];
    [self.devicePopup setAction:@selector(deviceChanged:)];
    [self refreshDevices];
    [view addSubview:self.devicePopup];
    
    // Start/Stop button
    self.startStopButton = [[NSButton alloc] initWithFrame:NSMakeRect(20, 370, 200, 32)];
    [self.startStopButton setTitle:@"Start"];
    [self.startStopButton setBezelStyle:NSBezelStyleRounded];
    [self.startStopButton setTarget:self];
    [self.startStopButton setAction:@selector(startStopClicked:)];
    [view addSubview:self.startStopButton];
    
    // Refresh button
    NSButton *refreshButton = [[NSButton alloc] initWithFrame:NSMakeRect(20, 330, 200, 32)];
    [refreshButton setTitle:@"Refresh"];
    [refreshButton setBezelStyle:NSBezelStyleRounded];
    [refreshButton setTarget:self];
    [refreshButton setAction:@selector(refreshClicked:)];
    [view addSubview:refreshButton];
    
    // Settings sliders
    int yPos = 280;
    
    [self addSliderWithLabel:@"Radius" yPos:yPos value:self.appData->settings->radius max:1000 
                      action:@selector(radiusChanged:) slider:&_radiusSlider contentView:view];
    yPos -= 40;
    
    [self addSliderWithLabel:@"Distance" yPos:yPos value:self.appData->settings->space max:1000 
                      action:@selector(spaceChanged:) slider:&_spaceSlider contentView:view];
    yPos -= 40;
    
    [self addSliderWithLabel:@"Speed" yPos:yPos value:self.appData->stream->speed max:1000 
                      action:@selector(speedChanged:) slider:&_speedSlider contentView:view];
    yPos -= 40;
    
    // Color fields
    [self addTextFieldWithLabel:@"Red" yPos:yPos value:self.appData->settings->red 
                         action:@selector(redChanged:) field:&_redField contentView:view];
    yPos -= 40;
    
    [self addTextFieldWithLabel:@"Green" yPos:yPos value:self.appData->settings->green 
                         action:@selector(greenChanged:) field:&_greenField contentView:view];
    yPos -= 40;
    
    [self addTextFieldWithLabel:@"Blue" yPos:yPos value:self.appData->settings->blue 
                         action:@selector(blueChanged:) field:&_blueField contentView:view];
    yPos -= 40;
    
    [self addTextFieldWithLabel:@"Alpha" yPos:yPos value:self.appData->settings->alpha 
                         action:@selector(alphaChanged:) field:&_alphaField contentView:view];
}

- (void)setupGifsTab {
    NSView *view = self.gifsTab;
    
    // Button bar at top
    NSButton *addGifButton = [[NSButton alloc] initWithFrame:NSMakeRect(20, 460, 100, 28)];
    [addGifButton setTitle:@"+ Add GIF"];
    [addGifButton setBezelStyle:NSBezelStyleRounded];
    [addGifButton setTarget:self];
    [addGifButton setAction:@selector(addGifClicked:)];
    [view addSubview:addGifButton];
    
    NSButton *loadGifsButton = [[NSButton alloc] initWithFrame:NSMakeRect(130, 460, 100, 28)];
    [loadGifsButton setTitle:@"Load GIFs"];
    [loadGifsButton setBezelStyle:NSBezelStyleRounded];
    [loadGifsButton setTarget:self];
    [loadGifsButton setAction:@selector(loadGifsClicked:)];
    [view addSubview:loadGifsButton];
    
    NSButton *saveGifsButton = [[NSButton alloc] initWithFrame:NSMakeRect(240, 460, 100, 28)];
    [saveGifsButton setTitle:@"Save GIFs"];
    [saveGifsButton setBezelStyle:NSBezelStyleRounded];
    [saveGifsButton setTarget:self];
    [saveGifsButton setAction:@selector(saveGifsClicked:)];
    [view addSubview:saveGifsButton];
    
    // Tab view for individual GIF tabs
    self.gifsTabView = [[NSTabView alloc] initWithFrame:NSMakeRect(10, 10, 660, 440)];
    [view addSubview:self.gifsTabView];
    
    // Load existing GIFs
    [self refreshGifTabs];
}

- (void)refreshGifTabs {
    // Clear all tabs
    while ([self.gifsTabView numberOfTabViewItems] > 0) {
        [self.gifsTabView removeTabViewItem:[self.gifsTabView tabViewItemAtIndex:0]];
    }
    
    // Add tab for each GIF
    if (self.appData->gifs) {
        GList *iter = self.appData->gifs;
        while (iter) {
            Gif *gif = (Gif*)iter->data;
            if (gif) {
                [self addGifTab:gif];
            }
            iter = g_list_next(iter);
        }
    }
}

- (void)addGifTab:(Gif*)gif {
    NSView *tabContent = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 640, 400)];
    
    // Get filename from path
    const char *filename = strrchr(gif->path, '/');
    filename = filename ? filename + 1 : gif->path;
    NSString *tabTitle = [NSString stringWithUTF8String:filename];
    
    NSTabViewItem *item = [[NSTabViewItem alloc] initWithIdentifier:[NSValue valueWithPointer:gif]];
    [item setLabel:tabTitle];
    [item setView:tabContent];
    
    // Left side: controls
    int yPos = 350;
    int xPos = 20;
    
    // X Position
    NSTextField *xLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(xPos, yPos, 100, 20)];
    [xLabel setStringValue:@"X Position"];
    [xLabel setBezeled:NO];
    [xLabel setDrawsBackground:NO];
    [xLabel setEditable:NO];
    [tabContent addSubview:xLabel];
    
    NSTextField *xField = [[NSTextField alloc] initWithFrame:NSMakeRect(xPos + 110, yPos, 80, 24)];
    [xField setIntValue:gif->gif_x];
    [xField setTag:(NSInteger)gif];
    [xField setTarget:self];
    [xField setAction:@selector(gifXChanged:)];
    [tabContent addSubview:xField];
    yPos -= 35;
    
    // Y Position
    NSTextField *yLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(xPos, yPos, 100, 20)];
    [yLabel setStringValue:@"Y Position"];
    [yLabel setBezeled:NO];
    [yLabel setDrawsBackground:NO];
    [yLabel setEditable:NO];
    [tabContent addSubview:yLabel];
    
    NSTextField *yField = [[NSTextField alloc] initWithFrame:NSMakeRect(xPos + 110, yPos, 80, 24)];
    [yField setIntValue:gif->gif_y];
    [yField setTag:(NSInteger)gif];
    [yField setTarget:self];
    [yField setAction:@selector(gifYChanged:)];
    [tabContent addSubview:yField];
    yPos -= 35;
    
    // Width
    NSTextField *wLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(xPos, yPos, 100, 20)];
    [wLabel setStringValue:@"Width"];
    [wLabel setBezeled:NO];
    [wLabel setDrawsBackground:NO];
    [wLabel setEditable:NO];
    [tabContent addSubview:wLabel];
    
    NSTextField *wField = [[NSTextField alloc] initWithFrame:NSMakeRect(xPos + 110, yPos, 80, 24)];
    [wField setIntValue:gif->gif_width];
    [wField setTag:(NSInteger)gif];
    [wField setTarget:self];
    [wField setAction:@selector(gifWidthChanged:)];
    [tabContent addSubview:wField];
    yPos -= 35;
    
    // Height
    NSTextField *hLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(xPos, yPos, 100, 20)];
    [hLabel setStringValue:@"Height"];
    [hLabel setBezeled:NO];
    [hLabel setDrawsBackground:NO];
    [hLabel setEditable:NO];
    [tabContent addSubview:hLabel];
    
    NSTextField *hField = [[NSTextField alloc] initWithFrame:NSMakeRect(xPos + 110, yPos, 80, 24)];
    [hField setIntValue:gif->gif_height];
    [hField setTag:(NSInteger)gif];
    [hField setTarget:self];
    [hField setAction:@selector(gifHeightChanged:)];
    [tabContent addSubview:hField];
    yPos -= 35;
    
    // Close button
    NSButton *closeButton = [[NSButton alloc] initWithFrame:NSMakeRect(xPos, yPos - 10, 190, 32)];
    [closeButton setTitle:@"Remove GIF"];
    [closeButton setBezelStyle:NSBezelStyleRounded];
    [closeButton setTag:(NSInteger)gif];
    [closeButton setTarget:self];
    [closeButton setAction:@selector(removeGifClicked:)];
    [tabContent addSubview:closeButton];
    
    // Right side: GIF preview
    NSImageView *imageView = [[NSImageView alloc] initWithFrame:NSMakeRect(250, 50, 350, 300)];
    [imageView setImageScaling:NSImageScaleProportionallyUpOrDown];
    
    // Load GIF into NSImage
    NSString *gifPath = [NSString stringWithUTF8String:gif->path];
    NSImage *gifImage = [[NSImage alloc] initWithContentsOfFile:gifPath];
    if (gifImage) {
        [imageView setImage:gifImage];
    }
    [tabContent addSubview:imageView];
    
    [self.gifsTabView addTabViewItem:item];
}

- (void)addSliderWithLabel:(NSString*)label yPos:(int)y value:(double)value max:(double)max
                    action:(SEL)action slider:(NSSlider**)slider contentView:(NSView*)view {
    NSTextField *labelField = [[NSTextField alloc] initWithFrame:NSMakeRect(20, y, 100, 24)];
    [labelField setStringValue:label];
    [labelField setBezeled:NO];
    [labelField setDrawsBackground:NO];
    [labelField setEditable:NO];
    [view addSubview:labelField];
    
    *slider = [[NSSlider alloc] initWithFrame:NSMakeRect(130, y, 450, 24)];
    [*slider setMinValue:0];
    [*slider setMaxValue:max];
    [*slider setDoubleValue:value];
    [*slider setTarget:self];
    [*slider setAction:action];
    [view addSubview:*slider];
}

- (void)addTextFieldWithLabel:(NSString*)label yPos:(int)y value:(double)value
                        action:(SEL)action field:(NSTextField**)field contentView:(NSView*)view {
    NSTextField *labelField = [[NSTextField alloc] initWithFrame:NSMakeRect(20, y, 100, 24)];
    [labelField setStringValue:label];
    [labelField setBezeled:NO];
    [labelField setDrawsBackground:NO];
    [labelField setEditable:NO];
    [view addSubview:labelField];
    
    *field = [[NSTextField alloc] initWithFrame:NSMakeRect(130, y, 100, 24)];
    [*field setDoubleValue:value];
    [*field setTarget:self];
    [*field setAction:action];
    [view addSubview:*field];
}

- (void)refreshDevices {
    [self.devicePopup removeAllItems];
    
    int cnt = Pa_GetDeviceCount();
    for (int dev = 0; dev < cnt; dev++) {
        const PaDeviceInfo* device_info = Pa_GetDeviceInfo(dev);
        if (device_info->maxInputChannels > 0 && device_info->maxInputChannels < 8) {
            NSString *name = [NSString stringWithUTF8String:device_info->name];
            [self.devicePopup addItemWithTitle:name];
            [[self.devicePopup lastItem] setTag:dev];
        }
    }
}

- (void)deviceChanged:(id)sender {
    NSInteger tag = [[self.devicePopup selectedItem] tag];
    device = (int)tag;
    const PaDeviceInfo* di = Pa_GetDeviceInfo(device);
    if (di) {
        [self.deviceLabel setStringValue:[NSString stringWithUTF8String:di->name]];
    }
}

- (void)startStopClicked:(id)sender {
    if (self.appData->visualizer == false) {
        if(device < 0 || device > Pa_GetDeviceCount()) {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setMessageText:@"Select valid source"];
            [alert runModal];
            return;
        }
        self.appData->visualizer = true;
        self.appData->stream->device = device;
        start_stream(self.appData->stream);
        const PaDeviceInfo* di = Pa_GetDeviceInfo(device);
        if (di) {
            [self.deviceLabel setStringValue:[NSString stringWithUTF8String:di->name]];
        }
        [self.startStopButton setTitle:@"Stop"];
    } else {
        [self.startStopButton setTitle:@"Start"];
        close_stream(self.appData->stream);
        self.appData->visualizer = false;
    }
}

- (void)refreshClicked:(id)sender {
    if(self.appData->visualizer) {
        [self startStopClicked:nil];
    }
    refresh_stream(self.appData->stream);
    [self refreshDevices];
    device = -1;
}

- (void)radiusChanged:(id)sender {
    self.appData->settings->radius = [self.radiusSlider doubleValue];
    write_config(self.appData->settings);
}

- (void)spaceChanged:(id)sender {
    self.appData->settings->space = [self.spaceSlider doubleValue];
    write_config(self.appData->settings);
}

- (void)speedChanged:(id)sender {
    double value = [self.speedSlider doubleValue];
    self.appData->stream->speed = value;
    self.appData->settings->speed = value;
    write_config(self.appData->settings);
}

- (void)redChanged:(id)sender {
    self.appData->settings->red = [self.redField doubleValue];
    write_config(self.appData->settings);
}

- (void)greenChanged:(id)sender {
    self.appData->settings->green = [self.greenField doubleValue];
    write_config(self.appData->settings);
}

- (void)blueChanged:(id)sender {
    self.appData->settings->blue = [self.blueField doubleValue];
    write_config(self.appData->settings);
}

- (void)alphaChanged:(id)sender {
    self.appData->settings->alpha = [self.alphaField doubleValue];
    write_config(self.appData->settings);
}

- (void)gifXChanged:(id)sender {
    NSTextField *field = (NSTextField*)sender;
    Gif *gif = (Gif*)[field tag];
    gif->gif_x = [field intValue];
}

- (void)gifYChanged:(id)sender {
    NSTextField *field = (NSTextField*)sender;
    Gif *gif = (Gif*)[field tag];
    gif->gif_y = [field intValue];
}

- (void)gifWidthChanged:(id)sender {
    NSTextField *field = (NSTextField*)sender;
    Gif *gif = (Gif*)[field tag];
    gif->gif_width = [field intValue];
}

- (void)gifHeightChanged:(id)sender {
    NSTextField *field = (NSTextField*)sender;
    Gif *gif = (Gif*)[field tag];
    gif->gif_height = [field intValue];
}

- (void)removeGifClicked:(id)sender {
    NSButton *button = (NSButton*)sender;
    Gif *gif = (Gif*)[button tag];
    
    // Remove from list
    self.appData->gifs = g_list_remove(self.appData->gifs, gif);
    
    // Free memory
    gif_free(gif);
    
    // Refresh tabs
    [self refreshGifTabs];
}

- (void)addGifClicked:(id)sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories:NO];
    [panel setAllowsMultipleSelection:NO];
    [panel setAllowedFileTypes:@[@"gif"]];
    
    [panel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
        if (result == NSModalResponseOK) {
            NSURL *url = [[panel URLs] firstObject];
            const char *path = [[url path] UTF8String];
            
            Gif* gif = gif_new(path);
            if (gif->gif_width > 512 || gif->gif_height > 512) {
                NSAlert *alert = [[NSAlert alloc] init];
                [alert setMessageText:[NSString stringWithFormat:@"GIF is too large!\nDimensions: %dx%d\nMax allowed: 512x512", 
                                      gif->gif_width, gif->gif_height]];
                [alert runModal];
                gif_free(gif);
                return;
            }
            
            self.appData->gifs = g_list_append(self.appData->gifs, gif);
            [self refreshGifTabs];
        }
    }];
}

- (void)loadGifsClicked:(id)sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories:NO];
    [panel setAllowsMultipleSelection:NO];
    [panel setAllowedFileTypes:@[@"uwu"]];
    
    [panel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
        if (result == NSModalResponseOK) {
            NSURL *url = [[panel URLs] firstObject];
            const char *path = [[url path] UTF8String];
            load_gifs(path, self.appData);
            [self refreshGifTabs];
        }
    }];
}

- (void)saveGifsClicked:(id)sender {
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setAllowedFileTypes:@[@"uwu"]];
    [panel setNameFieldStringValue:@"gifs.uwu"];
    
    [panel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
        if (result == NSModalResponseOK) {
            NSURL *url = [panel URL];
            const char *path = [[url path] UTF8String];
            save_gifs(path, self.appData);
        }
    }];
}

@end

void* macos_create_settings_window(void* app_data) {
    AppData* data = (AppData*)app_data;
    SettingsWindowController *controller = [[SettingsWindowController alloc] initWithAppData:data];
    return (__bridge_retained void*)controller;
}

void macos_show_settings_window(void* window) {
    SettingsWindowController *controller = (__bridge SettingsWindowController*)window;
    [controller showWindow:nil];
    [[controller window] makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

void macos_destroy_settings_window(void* window) {
    SettingsWindowController *controller = (__bridge_transfer SettingsWindowController*)window;
    [controller close];
}
