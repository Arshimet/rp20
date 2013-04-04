//
//  SongAdder.h
//  RP HD
//
//  Created by Giacomo Tufano on 04/09/12.
//
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

#import "Song.h"

@interface SongAdder : NSObject

@property (nonatomic, retain) NSString * title;
@property (nonatomic, retain) NSString * artist;
@property (nonatomic, retain) NSDate * dateadded;
@property (nonatomic, retain) NSString * sha;
@property (nonatomic, retain) NSData *cover;

-(id)initWithTitle:(NSString *)title andArtist:(NSString *)artist andCoversheet:(NSImage *)cover;
-(BOOL)addSong:(NSError **)outError;

@end