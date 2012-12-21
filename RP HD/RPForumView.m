//
//  RPForumView.m
//  RP HD
//
//  Created by Giacomo Tufano on 16/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "RPForumView.h"

#import "LocalyticsSession.h"

@interface RPForumView () <UIWebViewDelegate, UIActionSheetDelegate>

@property (nonatomic, strong, readonly) UIActionSheet *pageActionSheet;

@end

@implementation RPForumView

@synthesize theWebView = _theWebView;
@synthesize goBackButton = _goBackButton;
@synthesize goForwardButton = _goForwardButton;
@synthesize refreshButton = _refreshButton;
@synthesize actionButton = _actionButton;
@synthesize theSpinner = _theSpinner;
@synthesize pageActionSheet = _pageActionSheet;
@synthesize songId = _songId;

- (UIActionSheet *)pageActionSheet 
{    
    if(!_pageActionSheet) 
    {
        _pageActionSheet = [[UIActionSheet alloc] initWithTitle:self.theWebView.request.URL.absoluteString delegate:self cancelButtonTitle:nil destructiveButtonTitle:nil otherButtonTitles:nil]; 
        [_pageActionSheet addButtonWithTitle:NSLocalizedString(@"Copy Link", @"")];
        [_pageActionSheet addButtonWithTitle:NSLocalizedString(@"Open in Safari", @"")];
        [_pageActionSheet addButtonWithTitle:NSLocalizedString(@"Cancel", @"")];
        _pageActionSheet.cancelButtonIndex = [_pageActionSheet numberOfButtons]-1;
    }
    return _pageActionSheet;
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    DLog(@"webloadStarted");
    [self.theSpinner startAnimating];
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    DLog(@"page Loaded");
    self.theWebView.hidden = NO;
    [self.theSpinner stopAnimating];
    self.goForwardButton.enabled = [self.theWebView canGoForward];
    self.goBackButton.enabled = [self.theWebView canGoBack];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Don't show the ugly white window... :)
    self.theWebView.hidden = YES;
    self.isForumShown = YES;
    NSString *url = [NSString stringWithFormat:kRPCurrentSongForum, self.songId];
    [self.theWebView loadRequest:[[NSURLRequest alloc] initWithURL:[NSURL URLWithString:url]]];
    [[LocalyticsSession sharedLocalyticsSession] tagEvent:@"ForumOpened"];
}

- (void)viewDidUnload
{
    [self setTheWebView:nil];
    [self setGoBackButton:nil];
    [self setGoForwardButton:nil];
    [self setRefreshButton:nil];
    [self setActionButton:nil];
    [self setTheSpinner:nil];
    [self setLyricsButton:nil];
    [super viewDidUnload];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;
}

- (IBAction)viewIsDone:(id)sender 
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)goBackClicked:(id)sender 
{
    [self.theWebView goBack];
}

- (IBAction)goForwardClicked:(id)sender 
{
    [self.theWebView goForward];
}

- (IBAction)refreshClicked:(id)sender 
{
    [self.theWebView reload];
}

- (IBAction)actionClicked:(id)sender 
{
    [self.pageActionSheet showFromBarButtonItem:self.actionButton animated:YES];
}

- (IBAction)loadLyrics:(id)sender
{
    if(self.isForumShown)
    {
        [[LocalyticsSession sharedLocalyticsSession] tagEvent:@"lyricsClicked"];
        if(self.theWebView.isLoading)
            [self.theWebView stopLoading];
        DLog(@"Metadata info for lyrics is %@", self.currentSongName);
        // URL-encode song and artist names
        NSString *temp = self.currentSongName;
        temp = [temp stringByReplacingOccurrencesOfString:@" " withString:@"+"];
        temp = [temp stringByReplacingOccurrencesOfString:@"@" withString:@"%40"];
        temp = [temp stringByReplacingOccurrencesOfString:@"?" withString:@"%3F"];
        temp = [temp stringByReplacingOccurrencesOfString:@":" withString:@"%3A"];
        temp = [temp stringByReplacingOccurrencesOfString:@"&" withString:@"%26"];
        NSString *baseDDGUrl;
        baseDDGUrl = @"https://duckduckgo.com/?q=\\lyrics+%@";
        NSString *searchURL = [NSString stringWithFormat:baseDDGUrl, temp];
        searchURL = [searchURL stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        DLog(@"Loading lyrics from <%@>", searchURL);
        [self.lyricsButton setTitle:@"Forum"];
        self.isForumShown = NO;
        [self.theWebView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:searchURL]]];
    }
    else
    {
        DLog(@"Get back to forum view");
        [self.lyricsButton setTitle:@"Lyrics"];
        self.isForumShown = YES;
        NSString *url = [NSString stringWithFormat:kRPCurrentSongForum, self.songId];
        [self.theWebView loadRequest:[[NSURLRequest alloc] initWithURL:[NSURL URLWithString:url]]];
    }
}

#pragma mark -
#pragma mark UIActionSheetDelegate

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	NSString *title = [actionSheet buttonTitleAtIndex:buttonIndex];
    
	if([title isEqualToString:NSLocalizedString(@"Open in Safari", @"")])
        [[UIApplication sharedApplication] openURL:self.theWebView.request.URL];
    
    if([title isEqualToString:NSLocalizedString(@"Copy Link", @"")]) 
    {
        UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
        pasteboard.string = self.theWebView.request.URL.absoluteString;
    }
    _pageActionSheet = nil;
}

@end
